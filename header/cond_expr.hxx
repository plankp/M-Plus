#ifndef _MP_COND_EXPR_HXX__
#define _MP_COND_EXPR_HXX__

#include "visitor.hxx"

#include <memory>
#include <vector>
#include <algorithm>

namespace syntree
{
  class cond_expr : public ast
  {
  private:
    std::vector<std::shared_ptr<syntree::ast>> if_clauses;
    std::shared_ptr<syntree::ast> else_clause;

  public:
    cond_expr() = default;
    cond_expr(std::vector<std::shared_ptr<syntree::ast>> ifs,
	      std::shared_ptr<syntree::ast> else_);
    cond_expr(const syntree::cond_expr &ref);
    cond_expr(syntree::cond_expr &&mref);
    ~cond_expr() = default;

    syntree::cond_expr &operator=(syntree::cond_expr obj);

    void push_if_node(std::shared_ptr<syntree::ast> if_node);
    void set_else_node(std::shared_ptr<syntree::ast> else_node);

    size_t count_ifs(void);
    std::shared_ptr<syntree::ast> get_ifs(size_t idx);
    std::shared_ptr<syntree::ast> get_else(void);

    virtual void accept(visitor &v);
    virtual std::string type_name(void) const;
    virtual std::unique_ptr<rt::mp_value> eval(env_t env);
    virtual std::unique_ptr<rt::mp_value> clone(void) const;
    virtual std::unique_ptr<rt::mp_value> send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param);

    friend void swap(syntree::cond_expr &a, syntree::cond_expr &b);
  };
};

#endif /* !_MP_COND_EXPR_HXX__ */
