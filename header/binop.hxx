#ifndef _MP_BINOP_HXX__
#define _MP_BINOP_HXX__

#include "token.hxx"
#include "visitor.hxx"

#include "quote_expr.hxx"

#include <memory>
#include <algorithm>

namespace syntree
{
  class binop : public ast
  {
  private:
    mp_token_t op;
    std::shared_ptr<ast> lhs;
    std::shared_ptr<ast> rhs;

  public:
    binop() = default;
    binop(mp_token_t op, std::shared_ptr<ast> lhs, std::shared_ptr<ast> rhs);
    binop(const syntree::binop &ref);
    binop(syntree::binop &&mref);
    ~binop() = default;

    syntree::binop &operator=(syntree::binop obj);

    mp_token_t get_op(void);
    std::shared_ptr<ast> get_lhs(void);
    std::shared_ptr<ast> get_rhs(void);

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);

    friend void swap(syntree::binop &a, syntree::binop &b);
  };
};

#endif /* !_MP_BINOP_HXX__ */
