#ifndef _MP_EXPRESSIONS_HXX__
#define _MP_EXPRESSIONS_HXX__

#include "visitor.hxx"

#include <memory>
#include <vector>
#include <algorithm>

namespace syntree
{
  class expressions : public ast
  {
  private:
    std::vector<std::shared_ptr<ast>> exprs;
  public:
    expressions(std::vector<std::shared_ptr<ast>> exprs = {});
    expressions(const syntree::expressions &ref);
    expressions(syntree::expressions &&ref);
    ~expressions();

    syntree::expressions &operator=(syntree::expressions obj);
    std::shared_ptr<ast> &operator[](size_t idx);

    void push_back(std::shared_ptr<ast> ptr);
    size_t size(void) const;

    virtual void accept(visitor &vis);
    virtual std::string type_name(void) const;

    friend void swap(syntree::expressions &a, syntree::expressions &b);
  };
};

#endif /* !_MP_EXPRESSIONS_HXX__ */
