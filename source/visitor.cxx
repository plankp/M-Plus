#include "visitor.hxx"

void
visitor::visit(syntree::ast &tree)
{
  tree.accept(*this);
}
