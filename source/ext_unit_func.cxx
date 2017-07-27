#include "ext_unit_func.hxx"

namespace rt
{
  namespace ext
  {
    unit_func::unit_func(std::function<std::unique_ptr<rt::mp_value>(void)> _d)
      : function(), delegate(_d)
    {
    }

    unit_func::unit_func(const rt::ext::unit_func &ref)
      : function(ref), delegate(ref.delegate)
    {
    }

    unit_func::unit_func(rt::ext::unit_func &&mref)
      : unit_func()
    {
      swap(*this, mref);
    }

    rt::ext::unit_func &
    unit_func::operator=(rt::ext::unit_func obj)
    {
      swap(*this, obj);
      return *this;
    }

    std::unique_ptr<rt::mp_value>
    unit_func::clone(void) const
    {
      return std::unique_ptr<rt::mp_value>(new rt::ext::unit_func(*this));
    }

    std::unique_ptr<rt::mp_value>
    unit_func::on_call(void)
    {
      return delegate();
    }

    std::unique_ptr<rt::mp_value>
    unit_func::on_call(std::unique_ptr<rt::mp_value> param)
    {
      throw rt::dispatch_error(*this, "call", "external function has arity of 0");
    }

    void
    swap(rt::ext::unit_func &a, rt::ext::unit_func &b)
    {
      using std::swap;

      swap(static_cast<rt::function&>(a), static_cast<rt::function&>(b));
      swap(a.delegate, b.delegate);
    }
  };
};
