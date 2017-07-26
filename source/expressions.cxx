#include "expressions.hxx"

namespace syntree
{
  expressions::expressions(std::vector<std::shared_ptr<ast>> _exprs)
    : exprs(_exprs)
  {
  }

  expressions::expressions(const syntree::expressions &ref)
    : exprs(ref.exprs)
  {
  }

  expressions::expressions(syntree::expressions &&ref)
    : expressions()
  {
    swap(*this, ref);
  }

  syntree::expressions &
  expressions::operator=(syntree::expressions obj)
  {
    swap(*this, obj);
    return *this;
  }

  std::shared_ptr<ast> &
  expressions::operator[](size_t idx)
  {
    return exprs[idx];
  }

  void
  expressions::push_back(std::shared_ptr<ast> ptr)
  {
    exprs.push_back(ptr);
  }

  size_t
  expressions::size(void) const
  {
    return exprs.size();
  }

  void
  expressions::accept(visitor &v)
  {
    v.visit_expressions(*this);
  }

  std::string
  expressions::type_name(void) const
  {
    return "expressions";
  }
  
  std::unique_ptr<rt::mp_value>
  expressions::eval(env_t env)
  {
    std::unique_ptr<rt::mp_value> res = nullptr;
    for (size_t i = 0; i < exprs.size(); ++i) res = exprs[i]->eval(env);
    return res;
  }

  std::unique_ptr<rt::mp_value>
  expressions::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new expressions(exprs));
  }

  std::unique_ptr<rt::mp_value>
  expressions::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    return eval(env)->send(env, msg, std::move(param));
  }

  void
  swap(syntree::expressions &a, syntree::expressions &b)
  {
    using std::swap;

    swap(a.exprs, b.exprs);
  }
};
