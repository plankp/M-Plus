#include "ext_unary_func.hxx"

namespace rt
{
  namespace ext
  {
    unary_func::unary_func(std::function<std::unique_ptr<rt::mp_value>(std::unique_ptr<rt::mp_value>)> _d)
      : function(), delegate(_d)
    {
    }

    unary_func::unary_func(const rt::ext::unary_func &ref)
      : function(ref), delegate(ref.delegate)
    {
    }

    unary_func::unary_func(rt::ext::unary_func &&mref)
      : unary_func()
    {
      swap(*this, mref);
    }

    rt::ext::unary_func &
    unary_func::operator=(rt::ext::unary_func obj)
    {
      swap(*this, obj);
      return *this;
    }

    std::unique_ptr<rt::mp_value>
    unary_func::clone(void) const
    {
      return std::unique_ptr<rt::mp_value>(new rt::ext::unary_func(*this));
    }

    std::unique_ptr<rt::mp_value>
    unary_func::on_call(void)
    {
      throw rt::dispatch_error(*this, "call", "external function has arity of 1");
    }

    std::unique_ptr<rt::mp_value>
    unary_func::on_call(std::unique_ptr<rt::mp_value> param)
    {
      return delegate(std::move(param));
    }

    void
    swap(rt::ext::unary_func &a, rt::ext::unary_func &b)
    {
      using std::swap;

      swap(static_cast<rt::function&>(a), static_cast<rt::function&>(b));
      swap(a.delegate, b.delegate);
    }
  };
};
