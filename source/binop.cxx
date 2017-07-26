#include "binop.hxx"

namespace syntree
{
  binop::binop(mp_token_t _op, std::shared_ptr<ast> _lhs, std::shared_ptr<ast> _rhs)
    : op(_op), lhs(_lhs), rhs(_rhs)
  {
  }

  binop::binop(const syntree::binop &ref)
    : op(ref.op), lhs(ref.lhs), rhs(ref.rhs)
  {
  }

  binop::binop(syntree::binop &&mref)
    : binop()
  {
    swap(*this, mref);
  }

  mp_token_t
  binop::get_op(void)
  {
    return op;
  }

  std::shared_ptr<ast>
  binop::get_lhs(void)
  {
    return lhs;
  }

  std::shared_ptr<ast>
  binop::get_rhs(void)
  {
    return rhs;
  }

  void
  binop::accept(visitor &v)
  {
    v.visit_binop(*this);
  }

  std::string
  binop::type_name(void) const
  {
    return "binop";
  }

  std::unique_ptr<rt::mp_value>
  binop::eval(env_t env)
  {
    // Intercept special messages first!
    if (op.text == "if")
      {
	if (lhs->eval(env)->is_truthy()) return rhs->eval(env);
	return nullptr;
      }

    // Have lhs handle other messages
    return lhs->send(env, op.text, rhs->clone());
  }

  std::unique_ptr<rt::mp_value>
  binop::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new binop(op, lhs, rhs));
  }

  std::unique_ptr<rt::mp_value>
  binop::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    return eval(env)->send(env, msg, std::move(param));
  }

  void
  swap(syntree::binop &a, syntree::binop &b)
  {
    using std::swap;

    swap(a.op, b.op);
    swap(a.lhs, b.lhs);
    swap(a.rhs, b.rhs);
  }
};
