#ifndef __CONSTANTS_HH__
#define __CONSTANTS_HH__

#include <string_view>
#include <Utility.hh>

struct Constants {
  static constexpr std::string_view namespace_v = "tlang";
  static constexpr std::string_view kind_v = "ASTKind";
  static constexpr std::string_view parent_class_v = "ParentClasses";
  static constexpr std::string_view children_class_v = "ChildrenList";
  static constexpr std::string_view clasoff_v = "classof";

  static constexpr std::string_view base_type_v = "base_type";
  static constexpr std::string_view parents_v = "parent_list";
  static constexpr std::string_view offsets_v = "children_offsets";
  static constexpr std::string_view children_list_v = "children_list";
  static constexpr std::string_view children_v = "children";

  static std::string offset(const std::string& str) {
    return capitalize(str) + "Offset";
  }
};

using C = Constants;

#endif
