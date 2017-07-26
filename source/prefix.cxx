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

  std::string
  prefix::type_name(void) const
  {
    return "prefix";
  }

  std::unique_ptr<rt::mp_value>
  prefix::eval(env_t env)
  {
    return base->eval(env)->send(env, op.text, nullptr);
  }

  std::unique_ptr<rt::mp_value>
  prefix::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new prefix(op, base));
  }

  std::unique_ptr<rt::mp_value>
  prefix::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    return eval(env)->send(env, msg, std::move(param));
  }

  void
  swap(syntree::prefix &a, syntree::prefix &b)
  {
    using std::swap;

    swap(a.op, b.op);
    swap(a.base, b.base);
  }
};
