#ifndef _MP_EXT_UNIT_FUNC_HXX__
#define _MP_EXT_UNIT_FUNC_HXX__

#include "function.hxx"

#include <memory>
#include <algorithm>

namespace rt
{
  namespace ext
  {
    class unit_func : public rt::function
    {
    private:
      std::function<std::unique_ptr<rt::mp_value>(void)> delegate;

      unit_func() = default;

    public:
      unit_func(std::function<std::unique_ptr<rt::mp_value>(void)> delegate);
      unit_func(const rt::ext::unit_func &ref);
      unit_func(rt::ext::unit_func &&mref);
      ~unit_func() = default;

      rt::ext::unit_func &operator=(rt::ext::unit_func obj);

      virtual std::unique_ptr<rt::mp_value> clone(void) const;
      virtual std::unique_ptr<rt::mp_value> on_call(void);
      virtual std::unique_ptr<rt::mp_value> on_call(std::unique_ptr<rt::mp_value> param);

      friend void swap(rt::ext::unit_func &a, rt::ext::unit_func &b);
    };
  };
};

#endif /* !_MP_EXT_UNIT_FUNC_HXX__ */
