#include "fapply.hxx"

namespace syntree
{
  fapply::fapply(std::shared_ptr<ast> _b, std::shared_ptr<ast> _p)
    : base(_b), param(_p)
  {
  }

  fapply::fapply(const syntree::fapply &ref)
    : base(ref.base), param(ref.param)
  {
  }

  fapply::fapply(syntree::fapply &&mref)
    : fapply()
  {
    swap(*this, mref);
  }

  syntree::fapply &
  fapply::operator=(syntree::fapply obj)
  {
    swap(*this, obj);
    return *this;
  }

  std::shared_ptr<ast>
  fapply::get_base(void)
  {
    return base;
  }

  std::shared_ptr<ast>
  fapply::get_param(void)
  {
    return param;
  }

  void
  fapply::accept(visitor &v)
  {
    v.visit_fapply(*this);
  }

  std::string
  fapply::type_name(void) const
  {
    return "fapply";
  }

  void
  swap(syntree::fapply &a, syntree::fapply &b)
  {
    using std::swap;

    swap(a.base, b.base);
    swap(a.param, b.param);
  }
};
