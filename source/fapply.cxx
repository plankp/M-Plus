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

  std::unique_ptr<rt::mp_value>
  fapply::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new fapply(base, param));
  }

  std::unique_ptr<rt::mp_value>
  fapply::eval(env_t env)
  {
    if (param) return base->send(env, "call", param->clone());
    return base->send(env, "call", nullptr);
  }

  std::unique_ptr<rt::mp_value>
  fapply::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> _p)
  {
    if (!_p && msg == "&") return rt::make_quote({ clone() });
    return eval(env)->send(env, msg, std::move(_p));
  }

  void
  swap(syntree::fapply &a, syntree::fapply &b)
  {
    using std::swap;

    swap(a.base, b.base);
    swap(a.param, b.param);
  }
};
