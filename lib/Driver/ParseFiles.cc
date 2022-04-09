#include <deque>
#include <filesystem>
#include <unordered_set>
#include <Driver/Driver.hh>
#include <Lex/Lexer.hh>
#include <Parser/Parser.hh>
#include <Support/UniqueStream.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang::driver {
namespace {
struct ImportVisitor: ASTVisitor<ImportVisitor, VisitorPattern::preOrder> {
  ImportVisitor(std::deque<std::string> &queue) :
      queue(queue) {
  }
  visit_t visitASTNode(ASTNode *node) {
    if (isa<ModuleDecl>(node))
      return visit;
    else if (auto importDecl = dyn_cast<ImportDecl>(node))
      queue.push_back(importDecl->getModuleName());
    return skip;
  }
  std::deque<std::string> &queue;
};
struct PathEquality {
  bool operator()(const std::filesystem::path &lhs, const std::filesystem::path &rhs) const {
    return std::filesystem::equivalent(rhs, lhs);
  }
};
struct PathHash {
  std::size_t operator()(const std::filesystem::path &path) const {
    return std::filesystem::hash_value(path);
  }
};
struct FileDB {
  FileDB(ASTContext &context, std::filesystem::path outputFile, const std::vector<std::string> &inputFiles) :
      context(context), outputFile(outputFile) {
    fileQueue.insert(fileQueue.begin(), inputFiles.begin(), inputFiles.end());
    searchPaths.push_back(std::filesystem::current_path());
  }
  ModuleDecl* parseFile(const std::string &file) {
    std::cerr << "Parsing file: " << file << std::endl;
    auto fs = unique_fstream::open_istream(file);
    lex::Lexer lex(*fs);
    parser::Parser parser(lex);
    auto module = parser.parse(context, file);
    std::cerr << "Done parsing: " << file << std::endl;
    return module;
  }
  std::filesystem::path valid(const std::string &fileName) {
    using namespace std::filesystem;
    path file = path(fileName);
    if (file.is_absolute()) {
      if (exists(file)) {
        path canonical_path = canonical(file);
        auto result = fileList.insert(canonical_path);
        if (result.second)
          return canonical_path;
        return path { };
      }
    } else {
      for (auto &sp : searchPaths) {
        auto tmp = sp / file;
        if (exists(tmp)) {
          path canonical_path = canonical(tmp);
          auto result = fileList.insert(canonical_path);
          if (result.second)
            return canonical_path;
          return path { };
        }
      }
    }
    throw(std::runtime_error("Invalid input file: " + file.string()));
  }
  void parseUnit(UnitDecl *unit, std::filesystem::path file) {
    auto module = parseFile(file.string());
    context.addModule(unit, module);
    std::deque<std::string> fileQueue;
    ImportVisitor { fileQueue }.traverseModuleDecl(module);
    searchPaths.push_front(file.parent_path());
    while (fileQueue.size()) {
      auto file = fileQueue.front();
      fileQueue.pop_front();
      auto fileCC = valid(file);
      if (!fileCC.empty()) {
        auto module = parseFile(fileCC.string());
        context.addModule(unit, module);
        ImportVisitor { fileQueue }.traverseModuleDecl(module);
      }
    }
    searchPaths.pop_front();
  }
  int run() {
    std::filesystem::path path = std::filesystem::absolute(outputFile);
    auto unit = context.addUnit(path.filename().string(), UnitDecl::Generic);
    while (fileQueue.size()) {
      auto file = fileQueue.front();
      fileQueue.pop_front();
      auto fileCC = valid(file);
      if (!fileCC.empty())
        parseUnit(unit, fileCC);
    }
    return 0;
  }
  ASTContext &context;
  std::filesystem::path outputFile;
  std::unordered_set<std::filesystem::path, PathHash, PathEquality> fileList;
  std::deque<std::string> fileQueue;
  std::deque<std::filesystem::path> searchPaths;
};
}
int Driver::parseFiles() {
  if (cmdArguments.inputFiles.empty())
    return 1;
  FileDB db(context, std::filesystem::path { cmdArguments.outputFile }, cmdArguments.inputFiles);
  return db.run();
}
}
