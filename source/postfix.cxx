#include "postfix.hxx"

namespace syntree
{
  postfix::postfix(mp_token_t _op, std::shared_ptr<ast> _base)
    : op(_op), base(_base)
  {
  }

  postfix::postfix(const syntree::postfix &ref)
    : op(ref.op), base(ref.base)
  {
  }

  postfix::postfix(syntree::postfix &&mref)
    : postfix()
  {
    swap(*this, mref);
  }

  mp_token_t
  postfix::get_op(void)
  {
    return op;
  }

  std::shared_ptr<ast>
  postfix::get_base(void)
  {
    return base;
  }

  syntree::postfix &
  postfix::operator=(syntree::postfix obj)
  {
    swap(*this, obj);
    return *this;
  }

  void
  postfix::accept(visitor &v)
  {
    v.visit_postfix(*this);
  }

  void
  swap(syntree::postfix &a, syntree::postfix &b)
  {
    using std::swap;

    swap(a.op, b.op);
    swap(a.base, b.base);
  }
};
