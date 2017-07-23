#include "expressions.hxx"

namespace syntree
{
  expressions::expressions(std::vector<std::shared_ptr<ast>> _exprs)
    : exprs(_exprs)
  {
  }

  expressions::expressions(const syntree::expressions &ref)
    : exprs(ref.exprs)
  {
  }

  expressions::expressions(syntree::expressions &&ref)
    : expressions()
  {
    swap(*this, ref);
  }

  expressions::~expressions()
  {
  }

  syntree::expressions &
  expressions::operator=(syntree::expressions obj)
  {
    swap(*this, obj);
    return *this;
  }

  std::shared_ptr<ast> &
  expressions::operator[](size_t idx)
  {
    return exprs[idx];
  }

  void
  expressions::push_back(std::shared_ptr<ast> ptr)
  {
    exprs.push_back(ptr);
  }

  size_t
  expressions::size(void) const
  {
    return exprs.size();
  }

  void
  expressions::accept(visitor &v)
  {
    v.visit_expressions(*this);
  }

  std::string
  expressions::type_name(void) const
  {
    return "expressions";
  }

  void
  swap(syntree::expressions &a, syntree::expressions &b)
  {
    using std::swap;

    swap(a.exprs, b.exprs);
  }
};
