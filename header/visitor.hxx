#ifndef _MP_VISITOR_HXX__
#define _MP_VISITOR_HXX__

namespace syntree
{
  // Forward declare ast types
  class ast;
  class binop;
  class token;
  class fapply;
  class prefix;
  class postfix;
  class cons_arr;
  class cond_expr;
  class expressions;
};

#include "ast.hxx"
#include "binop.hxx"
#include "fapply.hxx"
#include "prefix.hxx"
#include "postfix.hxx"
#include "cons_arr.hxx"
#include "cond_expr.hxx"
#include "token_node.hxx"
#include "expressions.hxx"

class visitor
{
public:
  virtual void visit(syntree::ast &tree);

  virtual void visit_binop(syntree::binop &binop) = 0;
  virtual void visit_token(syntree::token &token) = 0;
  virtual void visit_fapply(syntree::fapply &fapply) = 0;
  virtual void visit_prefix(syntree::prefix &prefix) = 0;
  virtual void visit_postfix(syntree::postfix &postfix) = 0;
  virtual void visit_cons_arr(syntree::cons_arr &cons_arr) = 0;
  virtual void visit_cond_expr(syntree::cond_expr &cond_expr) = 0;
  virtual void visit_expressions(syntree::expressions &expressions) = 0;
};

#endif /* !_MP_VISITOR_HXX__ */
