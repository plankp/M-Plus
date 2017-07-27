#include "function.hxx"

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

    if (msg == "call")
      {
	// If p is null, choose the void on_call (takes no parameters)
	// otherwise pass the parameter in
	if (p) return on_call(p->eval(env));
	return on_call();
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
