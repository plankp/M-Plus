#include "quote_expr.hxx"

namespace rt
{
  quote_expr::quote_expr(std::shared_ptr<rt::mp_value> _v)
    : val(_v)
  {
  }

  quote_expr::quote_expr(const rt::quote_expr &ref)
    : rt::mp_value(ref), val(ref.val)
  {
  }

  quote_expr::quote_expr(rt::quote_expr &&mref)
    : quote_expr()
  {
    swap(*this, mref);
  }

  rt::quote_expr &
  quote_expr::operator=(rt::quote_expr obj)
  {
    swap(*this, obj);
    return *this;
  }

  std::unique_ptr<rt::mp_value>
  quote_expr::eval(env_t env)
  {
    return clone();
  }

  std::unique_ptr<rt::mp_value>
  quote_expr::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new quote_expr(*this));
  }

  std::unique_ptr<rt::mp_value>
  quote_expr::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    if (param)
      {
	// Currently supports no binary messages
      }
    else
      {
	if (msg == "&") return rt::make_quote({ clone() });
	if (msg == "call") return val->clone();
      }

    throw rt::dispatch_error(*this, msg);
  }

  std::string
  quote_expr::to_str(void) const
  {
    return "&(" + val->to_str() + ")";
  }

  void
  swap(rt::quote_expr &a, rt::quote_expr &b)
  {
    using std::swap;

    swap(static_cast<rt::mp_value&>(a), static_cast<rt::mp_value&>(b));
    swap(a.val, b.val);
  }

  std::unique_ptr<rt::quote_expr>
  make_quote(std::shared_ptr<rt::mp_value> val)
  {
    return std::unique_ptr<rt::quote_expr>(new rt::quote_expr(val));
  }
};
