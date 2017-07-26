#ifndef _MP_IDENT_NODE_HXX__
#define _MP_IDENT_NODE_HXX__

#include "token.hxx"
#include "visitor.hxx"
#include "mp_error.hxx"
#include "udef_func.hxx"

#include <algorithm>

namespace syntree
{
  class ident : public ast
  {
  private:
    mp_token_t tok;

  public:
    ident() = default;
    ident(mp_token_t tok);
    ident(const syntree::ident &ref);
    ident(syntree::ident &&ref);
    ~ident();

    syntree::ident &operator=(syntree::ident obj);

    mp_token_t get_token(void);
    
    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);
    virtual std::string to_str(void) const;

    friend void swap(syntree::ident &a, syntree::ident &b);
  };
};

#endif /* !_MP_IDENT_NODE_HXX__ */
