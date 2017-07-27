#ifndef _MP_MP_VALUE_HXX__
#define _MP_MP_VALUE_HXX__

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

#define RT_TYPE_TAGS				\
  X(INT)					\
  X(FLOAT)					\
  X(ATOM)					\
  X(ARRAY)					\
  X(FUNC)					\
  X(EXPR)

namespace rt
{
#define X(n) n,
  enum struct type_tag { /* Macro expansion here */ RT_TYPE_TAGS };
#undef X

  class mp_value
  {
  private:
    rt::type_tag tag;

  public:
    using env_t = std::map<std::string, std::shared_ptr<rt::mp_value>> &;

    mp_value(rt::type_tag tag = rt::type_tag::EXPR);
    mp_value(const rt::mp_value &ref);
    mp_value(rt::mp_value &&mref);
    virtual ~mp_value() = default;

    rt::type_tag get_type_tag(void) const;

    virtual std::unique_ptr<rt::mp_value> eval(env_t env) = 0;
    virtual std::unique_ptr<rt::mp_value> clone(void) const = 0;
    virtual bool is_truthy(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param = nullptr) = 0;
    virtual std::string to_str(void) const = 0;

    friend void swap(rt::mp_value &a, rt::mp_value &b);
  };
};

std::string to_string (const rt::type_tag &tag);

#endif /* !_MP_MP_VALUE_HXX__ */
