#ifndef _MP_ARRAY_HXX__
#define _MP_ARRAY_HXX__

#include "visitor.hxx"
#include "mp_error.hxx"

#include "mp_int.hxx"

#include <deque>
#include <memory>
#include <algorithm>

namespace syntree
{
  class array : public ast
  {
  private:
    std::deque<std::shared_ptr<rt::mp_value>> data;
    
    std::unique_ptr<syntree::array> cov_clone(void) const;

  public:
    array(std::deque<std::shared_ptr<rt::mp_value>> data = {});
    array(const syntree::array &ref);
    array(syntree::array &&mref);
    ~array() = default;

    syntree::array &operator=(syntree::array obj);
    std::shared_ptr<rt::mp_value> &operator[](size_t idx);

    void push_back(std::shared_ptr<rt::mp_value> ptr);
    void push_front(std::shared_ptr<rt::mp_value> ptr);
    size_t size(void) const;

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual bool is_truthy(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);
    virtual std::string to_str(void) const;

    friend void swap(syntree::array &a, syntree::array &b);
  };
};

#endif /* !_MP_ARRAY_HXX__ */
