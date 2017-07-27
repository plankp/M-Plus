#include "function.hxx"

#include <iostream>

namespace rt
{
  function::function()
    : rt::mp_value(rt::type_tag::FUNC)
  {
  }

  function::function(const rt::function &ref)
    : rt::mp_value(ref)
  {
  }

  function::function(rt::function &&mref)
    : function()
  {
    swap(*this, mref);
  }

  std::unique_ptr<rt::mp_value>
  function::eval(env_t env)
  {
    return clone();
  }

  std::unique_ptr<rt::mp_value>
  function::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> p)
  {
    // The only thing that uses the env parameter
    // should be the parameter being passed

    if (p)
      {
	auto rhs = p->eval(env);
	if (msg == "call") return on_call(std::move(rhs));
	if (msg == ".")
	  {
	    // (f . g) => x -> f(g(x))
	    auto lhs_ptr = clone().release();
	    auto rhs_ptr = rhs.release();
	    return rt::ext::unary_func([lhs_ptr, rhs_ptr, &env](std::unique_ptr<rt::mp_value> x)
				       {
					 // C++11 does not support initializer in lambda capture, hence the raw_ptr->unique_ptr
					 auto lhs = std::unique_ptr<rt::mp_value>(lhs_ptr);
					 auto rhs = std::unique_ptr<rt::mp_value>(rhs_ptr);
					 return lhs->send(env, "call", rhs->send(env, "call", std::move(x)));
				       }).clone();
	  }
      }
    else
      {
	if (msg == "call") return on_call();
	if (msg == "&") return clone();
      }

    throw rt::dispatch_error(*this, msg);
  }

  std::string
  function::to_str(void) const
  {
    return "<function>";
  }

  void
  swap(rt::function &a, rt::function &b)
  {
    using std::swap;

    swap(static_cast<rt::mp_value&>(a), static_cast<rt::mp_value&>(b));
  }
};
