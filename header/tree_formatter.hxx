#ifndef _MP_TREE_FORMATTER_HXX__
#define _MP_TREE_FORMATTER_HXX__

#include "parser.hxx"

class tree_formatter : public visitor
{
private:
  std::stringstream sstr;
  size_t indent_level;
public:
  tree_formatter(void);
  ~tree_formatter() = default;

  void indent(void);
  void dedent(void);
  void newline(void);

  std::string get_text(void) const;

  virtual void visit_binop(syntree::binop &binop);
  virtual void visit_token(syntree::token &token);
  virtual void visit_fapply(syntree::fapply &fapply);
  virtual void visit_prefix(syntree::prefix &prefix);
  virtual void visit_postfix(syntree::postfix &postfix);
  virtual void visit_cons_arr(syntree::cons_arr &cons_arr);
  virtual void visit_cond_expr(syntree::cond_expr &cond_expr);
  virtual void visit_expressions(syntree::expressions &expressions);
};

#endif /* !_MP_TREE_FORMATTER_HXX__ */
