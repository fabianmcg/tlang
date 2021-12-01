#include "parser.hh"

namespace tlang::parser {

Parser::unique_t<QualType> Parser::handleType() {
  auto qualType = make_unique<QualType>();
  switch (tok.kind) {
  case tok_k::Int: {
    auto type = make_unique<BuiltinType>();
    type->getPrecision() = type->Int_64;
    type->getSignedness() = type->Signed;
    type->getKind() = type->Int;
    qualType->setType(std::move(type));
    consumeToken();
    break;
  }
  case tok_k::Float: {
    auto type = make_unique<BuiltinType>();
    type->getPrecision() = type->Float_32;
    type->getSignedness() = type->Signed;
    type->getKind() = type->Float;
    qualType->setType(std::move(type));
    consumeToken();
    break;
  }
  default:
    break;
  }
  return qualType;
}
Parser::unique_t<ParDecl> Parser::handlePar() {
  auto par = make_unique<ParDecl>();
  auto type = handleType();
  par->setType(std::move(type));
  if (tok.is(tok_k::Identifier)) {
    par->setIdentifier(tok.value());
    consumeToken();
  }
  return par;
}
void Parser::handleParameters(FunctionDecl &function) {
  consumeToken();
  while (tok.valid())
    switch (tok.kind) {
    case tok_k::RParen:
      consumeToken();
      return;
    case tok_k::Comma:
      consumeToken();
      return;
    case tok_k::Int: {
      auto par = handlePar();
      if (par)
        function.pushArgs(std::move(par));
      return;
    }
    case tok_k::Float: {
      auto par = handlePar();
//      if (par)
//        function.pushArgs(std::move(par));
      return;
    }
    default:
      return;
    }
}
Parser::unique_t<FunctionDecl> Parser::handleFunction() {
  auto function = make_unique<FunctionDecl>();
  if (tok.is(tok_k::Identifier))
    function->getIdentifier() = tok.value();
  consumeToken();
  if (tok.is(tok_k::LParen))
    handleParameters(*function);
  return function;
}
void Parser::handleModule(ASTContext &context) {
  context = ASTContext::create("__unnamed__");
  while (tok.valid()) {
    switch (tok.kind) {
    case tok_k::Function: {
      consumeToken();
      auto F = handleFunction();
      if (F)
        (*context)->addDecl(std::move(F));
      break;
    }
    default: {
      consumeToken();
      break;
    }
    }
  }
}
ASTContext Parser::parse() {
  ASTContext context;
  handleModule(context);
  return context;
}
void Parser::consumeToken() {
  auto tmp = current_token;
  if (tmp != tokens.end() && (++tmp) != tokens.end()) {
    ++current_token;
    tok = *current_token;
  } else {
    tok = lexer.consume_token();
    current_token = tokens.insert(tokens.end(), tok);
  }
}
token Parser::peekToken() {
  auto tmp = current_token;
  if (tmp != tokens.end() && (++tmp) != tokens.end()) {
    return *tmp;
  } else {
    auto tmp = lexer.consume_token();
    tokens.push_back(tmp);
    return tmp;
  }
}
void Parser::acceptRule(tok_it_t it) {
  current_token = tokens.erase(it, tokens.end());
  if (tokens.size())
    current_token--;
  if (current_token != tokens.end())
    tok = *current_token;
}
void Parser::restoreCtx(tok_it_t it) {
  current_token = it;
  tok = *it;
}
}
