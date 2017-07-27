#ifndef _MP_MP_INT_HXX__
#define _MP_MP_INT_HXX__

#include "mp_error.hxx"
#include "mp_value.hxx"

#include "mp_float.hxx"

#include <cmath>
#include <memory>
#include <algorithm>

namespace rt
{
  class mpint : public mp_value
  {
  private:
    long long val;

  public:
    mpint(long long val = 0L);
    mpint(const rt::mpint &ref);
    mpint(rt::mpint &&mref);
    ~mpint() = default;

    rt::mpint &operator=(rt::mpint obj);
    long long to_int(void) const;

    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual bool is_truthy(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);
    virtual std::string to_str(void) const;

    friend void swap(rt::mpint &a, rt::mpint &b);
  };
};

#endif /* _MP_MP_INT_HXX__ */
