#include "prefix.hxx"

namespace syntree
{
  prefix::prefix(mp_token_t _op, std::shared_ptr<ast> _base)
    : op(_op), base(_base)
  {
  }

  prefix::prefix(const syntree::prefix &ref)
    : op(ref.op), base(ref.base)
  {
  }

  prefix::prefix(syntree::prefix &&mref)
    : prefix()
  {
    swap(*this, mref);
  }

  syntree::prefix &
  prefix::operator=(syntree::prefix obj)
  {
    swap(*this, obj);
    return *this;
  }

  mp_token_t
  prefix::get_op(void)
  {
    return op;
  }

  std::shared_ptr<ast>
  prefix::get_base(void)
  {
    return base;
  }

  void
  prefix::accept(visitor &v)
  {
    v.visit_prefix(*this);
  }

  void
  swap(syntree::prefix &a, syntree::prefix &b)
  {
    using std::swap;

    swap(a.op, b.op);
    swap(a.base, b.base);
  }
};
