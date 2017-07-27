#ifndef _MP_EXT_UNARY_FUNC_HXX__
#define _MP_EXT_UNARY_FUNC_HXX__

#include "function.hxx"

#include <memory>
#include <algorithm>

namespace rt
{
  namespace ext
  {
    class unary_func : public rt:: function
    {
    private:
      std::function<std::unique_ptr<rt::mp_value>(std::unique_ptr<rt::mp_value>)> delegate;

      unary_func() = default;

    public:
      unary_func(std::function<std::unique_ptr<rt::mp_value>(std::unique_ptr<rt::mp_value>)> delegate);
      unary_func(const rt::ext::unary_func &ref);
      unary_func(rt::ext::unary_func &&mref);
      ~unary_func() = default;

      rt::ext::unary_func &operator=(rt::ext::unary_func obj);

      virtual std::unique_ptr<rt::mp_value> clone(void) const;
      virtual std::unique_ptr<rt::mp_value> on_call(void);
      virtual std::unique_ptr<rt::mp_value> on_call(std::unique_ptr<rt::mp_value> param);

      friend void swap(rt::ext::unary_func &a, rt::ext::unary_func &b);
    };
  };
};

#endif /* !_MP_EXT_UNARY_FUNC_HXX__ */
