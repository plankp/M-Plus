#ifndef _MP_POSTFIX_HXX__
#define _MP_POSTFIX_HXX__

#include "token.hxx"
#include "visitor.hxx"

#include <memory>
#include <algorithm>

namespace syntree
{
  class postfix : public ast
  {
  private:
    mp_token_t op;
    std::shared_ptr<ast> base;
  public:
    postfix() = default;
    postfix(mp_token_t op, std::shared_ptr<ast> base);
    postfix(const syntree::postfix &ref);
    postfix(syntree::postfix &&mref);
    ~postfix() = default;

    syntree::postfix &operator=(syntree::postfix obj);

    mp_token_t get_op(void);
    std::shared_ptr<ast> get_base(void);

    virtual void accept(visitor &v);

    friend void swap(syntree::postfix &a, syntree::postfix &b);
  };
};

#endif /* !_MP_POSTFIX_HXX__ */
