#ifndef _MP_CONS_ARR_HXX__
#define _MP_CONS_ARR_HXX__

#include "visitor.hxx"
#include "mp_error.hxx"

#include <deque>
#include <memory>
#include <algorithm>

namespace syntree
{
  class cons_arr : public ast
  {
  private:
    std::deque<std::shared_ptr<rt::mp_value>> data;
    
    std::unique_ptr<syntree::cons_arr> cov_clone(void) const;

  public:
    cons_arr(std::deque<std::shared_ptr<rt::mp_value>> data = {});
    cons_arr(const syntree::cons_arr &ref);
    cons_arr(syntree::cons_arr &&mref);
    ~cons_arr() = default;

    syntree::cons_arr &operator=(syntree::cons_arr obj);

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual bool is_truthy(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);

    friend void swap(syntree::cons_arr &a, syntree::cons_arr &b);
  };
};

#endif /* !_MP_CONS_ARR_HXX__ */
