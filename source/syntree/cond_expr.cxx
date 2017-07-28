#include "cond_expr.hxx"

namespace syntree
{
  cond_expr::cond_expr(std::vector<std::shared_ptr<syntree::ast>> _i,
		       std::shared_ptr<syntree::ast> _e)
    : if_clauses(_i), else_clause(_e)
  {
  }

  cond_expr::cond_expr(const syntree::cond_expr &ref)
    : if_clauses(ref.if_clauses), else_clause(ref.else_clause)
  {
  }

  cond_expr::cond_expr(syntree::cond_expr &&mref)
    : cond_expr()
  {
    swap(*this, mref);
  }

  syntree::cond_expr &
  cond_expr::operator= (syntree::cond_expr obj)
  {
    swap(*this, obj);
    return *this;
  }

  void
  cond_expr::push_if_node(std::shared_ptr<syntree::ast> if_node)
  {
    if_clauses.push_back(if_node);
  }

  void
  cond_expr::set_else_node(std::shared_ptr<syntree::ast> else_node)
  {
    else_clause = else_node;
  }

  size_t
  cond_expr::count_ifs(void)
  {
    return if_clauses.size();
  }

  std::shared_ptr<syntree::ast>
  cond_expr::get_ifs(size_t idx)
  {
    return if_clauses[idx];
  }

  std::shared_ptr<syntree::ast>
  cond_expr::get_else(void)
  {
    return else_clause;
  }

  void
  cond_expr::accept(visitor &v)
  {
    v.visit_cond_expr(*this);
  }

  std::string
  cond_expr::type_name(void) const
  {
    return "cond_expr";
  }

  std::unique_ptr<rt::mp_value>
  cond_expr::eval(env_t env)
  {
    for (size_t i = 0; i < if_clauses.size(); ++i)
      {
	auto res = if_clauses[i]->eval(env);
	if (res) return res;
      }
    if (else_clause)
      {
	auto res = else_clause->eval(env);
	if (res) return res;
      }
    return nullptr;
  }

  std::unique_ptr<rt::mp_value>
  cond_expr::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new cond_expr(if_clauses, else_clause));
  }

  std::unique_ptr<rt::mp_value>
  cond_expr::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    return eval(env)->send(env, msg, std::move(param));
  }

  void
  swap(syntree::cond_expr &a, syntree::cond_expr &b)
  {
    using std::swap;

    swap(a.if_clauses, b.if_clauses);
    swap(a.else_clause, b.else_clause);
  }
};
