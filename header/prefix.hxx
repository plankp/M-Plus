#ifndef _MP_PREFIX_HXX__
#define _MP_PREFIX_HXX__

#include "token.hxx"
#include "visitor.hxx"

#include "quote_expr.hxx"

#include <memory>
#include <algorithm>

namespace syntree
{
  class prefix : public ast
  {
  private:
    mp_token_t op;
    std::shared_ptr<ast> base;

  public:
    prefix() = default;
    prefix(mp_token_t op, std::shared_ptr<ast> base);
    prefix(const syntree::prefix &ref);
    prefix(syntree::prefix &&mref);
    ~prefix() = default;

    syntree::prefix &operator=(syntree::prefix obj);

    mp_token_t get_op(void);
    std::shared_ptr<ast> get_base(void);

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);
    virtual std::string to_str(void) const;

    friend void swap(syntree::prefix &a, syntree::prefix &b);
  };
};

#endif /* !_MP_PREFIX_HXX__ */
