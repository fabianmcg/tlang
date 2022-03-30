#ifndef AST_VISITORS_VISITOROPTIONS_HH
#define AST_VISITORS_VISITOROPTIONS_HH

namespace tlang {
class ASTVisitorBase {
public:
  typedef enum {
    visit,
    skip,
    terminate
  } VisitStatus;
  using visit_t = VisitStatus;
};
enum class VisitorPattern {
  preOrder = 1,
  postOrder = 2,
  prePostOrder = 3,
  postWalk = 4,
};
inline constexpr VisitorPattern operator|(VisitorPattern x, VisitorPattern y) {
  return static_cast<VisitorPattern>(static_cast<int>(x) | static_cast<int>(y));
}
inline constexpr VisitorPattern operator&(VisitorPattern x, VisitorPattern y) {
  return static_cast<VisitorPattern>(static_cast<int>(x) & static_cast<int>(y));
}
inline constexpr bool isPreVisit(VisitorPattern x) {
  return (x & VisitorPattern::preOrder) == VisitorPattern::preOrder;
}
inline constexpr bool isPostVisit(VisitorPattern x) {
  return (x & VisitorPattern::postOrder) == VisitorPattern::postOrder;
}
inline constexpr bool hasPostWalk(VisitorPattern x) {
  return (x & VisitorPattern::postWalk) == VisitorPattern::postWalk;
}
}

#endif
