#ifndef _MP_MP_VALUE_HXX__
#define _MP_MP_VALUE_HXX__

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

namespace rt
{
  class mp_value
  {
  public:
    using env_t = std::map<std::string, std::shared_ptr<rt::mp_value>> &;

    virtual std::unique_ptr<rt::mp_value> eval(env_t env) = 0;
    virtual std::unique_ptr<rt::mp_value> clone(void) const = 0;
    virtual bool is_truthy(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param = nullptr) = 0;
    virtual std::string to_str(void) const = 0;
  };
};

#endif /* !_MP_MP_VALUE_HXX__ */
