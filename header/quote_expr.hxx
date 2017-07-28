#ifndef _MP_QUOTE_EXPR_HXX__
#define _MP_QUOTE_EXPR_HXX__

#include "mp_error.hxx"
#include "mp_value.hxx"

#include <memory>
#include <algorithm>

namespace rt
{
  class quote_expr : public mp_value
  {
  private:
    std::shared_ptr<rt::mp_value> val;

    quote_expr() = default;

  public:
    quote_expr(std::shared_ptr<rt::mp_value> val);
    quote_expr(const rt::quote_expr &ref);
    quote_expr(rt::quote_expr &&mref);
    ~quote_expr() = default;

    rt::quote_expr &operator=(rt::quote_expr obj);

    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param = nullptr);
    virtual std::string to_str(void) const;

    friend void swap(rt::quote_expr &a, rt::quote_expr &b);
  };

  std::unique_ptr<rt::quote_expr> make_quote(std::shared_ptr<rt::mp_value> val);
};

#endif /* !_MP_MP_VALUE_HXX__ */
