#ifndef _MP_FAPPLY_HXX__
#define _MP_FAPPLY_HXX__

#include "visitor.hxx"

#include <memory>
#include <algorithm>

namespace syntree
{
  class fapply : public ast
  {
  private:
    std::shared_ptr<ast> base;
    std::shared_ptr<ast> param;

  public:
    fapply() = default;
    fapply(std::shared_ptr<ast> base, std::shared_ptr<ast> param);
    fapply(const syntree::fapply &ref);
    fapply(syntree::fapply &&ref);
    ~fapply() = default;

    syntree::fapply &operator=(syntree::fapply obj);

    std::shared_ptr<ast> get_base(void);
    std::shared_ptr<ast> get_param(void);

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;

    friend void swap(syntree::fapply &a, syntree::fapply &b);
  };
};

#endif /* !_MP_FAPPLY_HXX__ */
