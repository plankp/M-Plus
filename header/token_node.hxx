#ifndef _MP_TOKEN_NODE_HXX__
#define _MP_TOKEN_NODE_HXX__

#include "token.hxx"
#include "visitor.hxx"

#include <algorithm>

namespace syntree
{
  class token : public ast
  {
  private:
    mp_token_t tok;
  public:
    token() = default;
    token(mp_token_t tok);
    token(const syntree::token &ref);
    token(syntree::token &&ref);
    ~token();

    syntree::token &operator=(syntree::token obj);

    mp_token_t get_token(void);
    
    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;

    friend void swap(syntree::token& a, syntree::token &b);
  };
};

#endif /* !_MP_TOKEN_NODE_HXX__ */
