#ifndef _MP_MP_FLOAT_HXX__
#define _MP_MP_FLOAT_HXX__

#include "mp_error.hxx"
#include "mp_value.hxx"

#include "mp_int.hxx"

#include <cmath>
#include <memory>
#include <algorithm>

namespace rt
{
  class mpfloat : public mp_value
  {
  private:
    double val;

  public:
    mpfloat(double val = 0);
    mpfloat(const rt::mpfloat &ref);
    mpfloat(rt::mpfloat &&mref);
    ~mpfloat() = default;

    rt::mpfloat &operator=(rt::mpfloat obj);
    double to_double(void) const;

    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual bool is_truthy(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);
    virtual std::string to_str(void) const;

    friend void swap(rt::mpfloat &a, rt::mpfloat &b);
  };
};

#endif /* _MP_MP_FLOAT_HXX__ */
