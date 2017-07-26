#ifndef _MP_VISITOR_HXX__
#define _MP_VISITOR_HXX__

#include "ast.hxx"

namespace syntree
{
  // Forward declare ast types
  class ast;
  class num;
  class atom;
  class binop;
  class ident;
  class fapply;
  class prefix;
  class postfix;
  class cons_arr;
  class cond_expr;
  class expressions;
};

class visitor
{
public:
  virtual void visit(syntree::ast &tree);

  virtual void visit_num(syntree::num &num) = 0;
  virtual void visit_atom(syntree::atom &atom) = 0;
  virtual void visit_binop(syntree::binop &binop) = 0;
  virtual void visit_ident(syntree::ident &ident) = 0;
  virtual void visit_fapply(syntree::fapply &fapply) = 0;
  virtual void visit_prefix(syntree::prefix &prefix) = 0;
  virtual void visit_postfix(syntree::postfix &postfix) = 0;
  virtual void visit_cons_arr(syntree::cons_arr &cons_arr) = 0;
  virtual void visit_cond_expr(syntree::cond_expr &cond_expr) = 0;
  virtual void visit_expressions(syntree::expressions &expressions) = 0;
};

#endif /* !_MP_VISITOR_HXX__ */
