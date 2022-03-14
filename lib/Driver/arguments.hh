#ifndef __ARGUMENTS_HH__
#define __ARGUMENTS_HH__

#include <iostream>
#include <string>
#include <boost/program_options.hpp>

struct arguments {
  boost::program_options::options_description opts;
  boost::program_options::positional_options_description pos_opts { };
  boost::program_options::variables_map vmap { };
  arguments() = default;
  virtual ~arguments() = default;
  arguments(arguments&&) = default;
  arguments(const arguments&) = default;
  arguments& operator=(arguments&&) = default;
  arguments& operator=(const arguments&) = default;
  arguments(const std::string &name, bool add_help = true) :
      opts(name) {
    if (add_help)
      opts.add_options()("help,h", "print help message");
  }
  arguments(const std::string &name, int col_width, bool add_help = true) :
      opts(name, col_width > 0 ? col_width : 90) {
    if (add_help)
      opts.add_options()("help,h", "print help message");
  }
  auto& add_sopt(const std::string &ids, const std::string &help) {
    return opts.add_options()(ids.c_str(), help.c_str());
  }
  template <typename T>
  auto& add_opt(const std::string &ids, boost::program_options::typed_value<T> *value, const std::string &help) {
    return opts.add_options()(ids.c_str(), value, help.c_str());
  }
  template <typename T>
  auto& add_opt(const std::string &ids, const std::string &help) {
    return opts.add_options()(ids.c_str(), boost::program_options::value<T>(), help).c_str();
  }
  template <typename T>
  auto& add_opt(const std::string &ids, T &storage, const std::string &help) {
    return opts.add_options()(ids.c_str(), boost::program_options::value<T>(&storage), help.c_str());
  }
  template <typename T>
  auto& add_opt(const std::string &ids, T &&value, const std::string &help) {
    return opts.add_options()(ids.c_str(), boost::program_options::value<T>()->default_value(value), help.c_str());
  }
  template <typename T>
  auto& add_opt(const std::string &ids, T &storage, T value, const std::string &help) {
    return opts.add_options()(ids.c_str(), boost::program_options::value<T>(&storage)->default_value(value), help.c_str());
  }
  auto& add_popt(const std::string &ids, int count) {
    return pos_opts.add(ids.c_str(), count);
  }
  virtual std::string help() const {
    std::ostringstream ost;
    ost << opts;
    return ost.str();
  }
  int parse(int argc, char **argv, bool notify = true) {
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(opts).positional(pos_opts).run(), vmap);
    if (notify)
      boost::program_options::notify(vmap);
    if (vmap.count("help")) {
      std::cout << help() << std::endl;
      return 1;
    }
    return 0;
  }
  void parse_file(const std::string &filename, bool notify = true, bool allow_unregistered = false) {
    boost::program_options::store(boost::program_options::parse_config_file(filename.c_str(), opts, allow_unregistered), vmap);
    if (notify)
      boost::program_options::notify(vmap);
  }
  void parse_file(boost::program_options::variables_map &vmap, const std::string &filename, bool notify = true, bool allow_unregistered =
      false) {
    boost::program_options::store(boost::program_options::parse_config_file(filename.c_str(), opts, allow_unregistered), vmap);
    if (notify)
      boost::program_options::notify(vmap);
  }
  auto count(const std::string &id) const {
    return vmap.count(id);
  }
  bool defaulted(const std::string &id) const {
    return vmap[id].defaulted();
  }
  template <typename T>
  T get(const std::string &id) const {
    return vmap[id].template as<T>();
  }
  std::ostream& print(std::ostream &ost) const {
    ost << help();
    return ost;
  }
  static auto flag_opt() {
    return boost::program_options::bool_switch();
  }
  static auto flag_opt(bool &flag) {
    return boost::program_options::bool_switch(&flag);
  }
};
inline std::ostream& operator<<(std::ostream &ost, const arguments &args) {
  return args.print(ost);
}
#endif
