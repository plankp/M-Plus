#ifndef _MP_FUNCTION_HXX__
#define _MP_FUNCTION_HXX__

#include "mp_error.hxx"
#include "mp_value.hxx"

namespace rt
{
  class function : public mp_value
  {
  public:
    function() = default;
    virtual ~function() = default;

    virtual std::unique_ptr<rt::mp_value> eval(env_t env) final;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param) final;
    virtual std::string to_str(void) const;

    virtual std::unique_ptr<rt::mp_value> clone(void) const = 0;
    virtual std::unique_ptr<rt::mp_value> on_call(void) = 0;
    virtual std::unique_ptr<rt::mp_value> on_call(std::unique_ptr<rt::mp_value> param) = 0;
  };
};

#endif /* !_MP_FUNCTION_HXX__ */
