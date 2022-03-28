#include <ASTNode.hh>

void ASTNode::analyze() {
  if (hasBase()) {
    auto baseNode = base();
    parentNodes.push_back(&*baseNode);
  }
  auto parentList = record.getSuperClasses();
  for (auto parent : parentList) {
    if (parent.first->getName().equals(AbstractNode::kind))
      continue;
    parentNodes.push_back(parent.first);
  }
  for (auto &value : record.getValues())
    if (auto record = getAsRecord(&value)) {
      if (Child::is(record))
        childrenNodes.push_back(Child { value.getName(), *record });
      else if (Variable::is(record))
        variables.push_back(Variable { value.getName(), *record });
      else if (ClassSection::is(record))
        codeSections.push_back(ClassSection { *record });
      else if (Enum::is(record))
        enumerations.push_back(Enum { value.getName(), *record });
    }
}
