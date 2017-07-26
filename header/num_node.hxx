#ifndef _MP_NUM_NODE_HXX__
#define _MP_NUM_NODE_HXX__

#include "token.hxx"
#include "visitor.hxx"
#include "mp_error.hxx"

#include <stdexcept>

namespace syntree
{
  class num : public ast
  {
  private:
    mp_token_t tok;

  public:
    num() = default;
    num(mp_token_t tok);
    num(const syntree::num &ref);
    num(syntree::num &&mref);
    ~num() = default;

    syntree::num &operator=(syntree::num obj);

    mp_token_t get_token(void);

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual bool is_truthy(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);
    virtual std::string to_str(void) const;

    friend void swap(syntree::num &a, syntree::num &b);
  };
};

#endif /* !_MP_NUM_NODE_HXX__ */
