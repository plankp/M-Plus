#include "function.hxx"

namespace rt
{
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
    throw rt::dispatch_error(msg);
  }

  std::string
  function::to_str(void) const
  {
    return "<function>";
  }
};
