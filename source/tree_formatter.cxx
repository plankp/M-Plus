#include "tree_formatter.hxx"

tree_formatter::tree_formatter(void)
  : sstr(), indent_level(0)
{
}

std::string
tree_formatter::get_text(void) const
{
  return sstr.str();
}

void
tree_formatter::indent(void)
{
  ++indent_level;
}

void
tree_formatter::dedent(void)
{
  if (indent_level > 0) --indent_level;
}

void
tree_formatter::newline(void)
{
  sstr << '\n';
  for (size_t i = 0; i < indent_level; ++i) sstr << "  ";
}

void
tree_formatter::visit_fapply(syntree::fapply &fapply)
{
  sstr << "fapply {";
  indent();
  newline();
  sstr << "base: ";
  visit(*fapply.get_base());
  newline();
  sstr << "param: ";
  
  auto param = fapply.get_param();
  if (param) visit(*param);
  else sstr << "()";

  dedent();
  newline();
  sstr << '}';
}

void
tree_formatter::visit_binop(syntree::binop &binop)
{
  sstr << "binop {";
  indent();
  newline();
  sstr << "op: " << to_string(binop.get_op());
  newline();
  sstr << "lhs: ";
  if (binop.get_lhs()) visit(*binop.get_lhs());
  else sstr << "()";
  newline();
  sstr << "rhs: ";
  if (binop.get_rhs()) visit(*binop.get_rhs());
  else sstr << "()";
  dedent();
  newline();
  sstr << '}';
}

void
tree_formatter::visit_prefix(syntree::prefix &prefix)
{
  sstr << "prefix {";
  indent();
  newline();
  sstr << "op: " << to_string(prefix.get_op());
  newline();
  sstr << "base: ";
  visit(*prefix.get_base());
  dedent();
  newline();
  sstr << '}';
}

void
tree_formatter::visit_cond_expr(syntree::cond_expr &cond_expr)
{
  sstr << "cond_expr {";
  indent();

  for (size_t i = 0; i < cond_expr.count_ifs(); ++i)
    {
      newline();
      visit(*cond_expr.get_ifs(i));
    }

  if (cond_expr.get_else())
    {
      newline();
      visit(*cond_expr.get_else());
    }

  dedent();
  newline();
  sstr << '}';
}

void
tree_formatter::visit_postfix(syntree::postfix &postfix)
{
  sstr << "postfix {";
  indent();
  newline();
  sstr << "op: " << to_string(postfix.get_op());
  newline();
  sstr << "base: ";
  visit(*postfix.get_base());
  dedent();
  newline();
  sstr << '}';
}

void
tree_formatter::visit_token(syntree::token &token)
{
  sstr << to_string(token.get_token());
}

void
tree_formatter::visit_cons_arr(syntree::cons_arr &cons_arr)
{
  sstr << '[' << ']';
}

void
tree_formatter::visit_expressions(syntree::expressions &exprs)
{
  sstr << "expressions {";
  indent();  
  for (size_t i = 0; i < exprs.size(); ++i)
    {
      newline();
      visit(*exprs[i]);
    }
  dedent();
  newline();
  sstr << '}';
}
