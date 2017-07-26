#include "udef_func.hxx"

namespace rt
{
  udef_func::udef_func(std::string _p,
		       std::shared_ptr<rt::mp_value> _b,
		       std::map<std::string, std::shared_ptr<rt::mp_value>> _l)
    : locals(_l), param_name(_p), body(_b)
  {
  }

  udef_func::udef_func(const rt::udef_func &ref)
    : locals(ref.locals), param_name(ref.param_name), body(ref.body)
  {
  }

  udef_func::udef_func(rt::udef_func &&mref)
    : udef_func()
  {
    swap(*this, mref);
  }

  rt::udef_func &
  udef_func::operator=(rt::udef_func obj)
  {
    swap(*this, obj);
    return *this;
  }

  std::unique_ptr<rt::mp_value>
  udef_func::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new udef_func(*this));
  }

  std::string
  udef_func::to_str(void) const
  {
    if (param_name.empty()) return "<function/0>";
    return "<function/1>";
  }

  std::unique_ptr<rt::mp_value>
  udef_func::on_call(void)
  {
    if (param_name.empty()) return body->eval(locals);
    throw rt::dispatch_error("call [missing parameter <" + param_name + ">]");
  }

  std::unique_ptr<rt::mp_value>
  udef_func::on_call(std::unique_ptr<rt::mp_value> param)
  {
    if (param_name.empty()) throw rt::dispatch_error("call [unexpected parameter on unit function]");
    locals[param_name] = { std::move(param) };
    return body->eval(locals);
  }

  void
  swap(rt::udef_func &a, rt::udef_func &b)
  {
    using std::swap;

    swap(a.locals, b.locals);
    swap(a.param_name, b.param_name);
    swap(a.body, b.body);
  }
};
