#ifndef _MP_FUNCTION_HXX__
#define _MP_FUNCTION_HXX__

#include "mp_error.hxx"
#include "mp_value.hxx"

#include <algorithm>

namespace rt
{
  class function : public mp_value
  {
  public:
    function();
    function(const rt::function &ref);
    function(rt::function &&mref);
    virtual ~function() = default;

    virtual std::unique_ptr<rt::mp_value> eval(env_t env) final;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param) final;
    virtual std::string to_str(void) const;

    virtual std::unique_ptr<rt::mp_value> clone(void) const = 0;
    virtual std::unique_ptr<rt::mp_value> on_call(void) = 0;
    virtual std::unique_ptr<rt::mp_value> on_call(std::unique_ptr<rt::mp_value> param) = 0;

    friend void swap(rt::function &a, rt::function &b);
  };
};

#endif /* !_MP_FUNCTION_HXX__ */
