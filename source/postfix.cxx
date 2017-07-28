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

  std::string
  postfix::type_name(void) const
  {
    return "postfix";
  }

  std::unique_ptr<rt::mp_value>
  postfix::eval(env_t env)
  {
    // Intercept special messages first!
    if (op.text == "else") return base->eval(env);

    // Have base handle other messages
    return base->eval(env)->send(env, op.text, nullptr);
  }

  std::unique_ptr<rt::mp_value>
  postfix::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new postfix(op, base));
  }

  std::unique_ptr<rt::mp_value>
  postfix::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    if (!param && msg == "&") return rt::make_quote({ clone() });
    return eval(env)->send(env, msg, std::move(param));
  }

  void
  swap(syntree::postfix &a, syntree::postfix &b)
  {
    using std::swap;

    swap(a.op, b.op);
    swap(a.base, b.base);
  }
};
