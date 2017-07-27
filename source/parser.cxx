#include "parser.hxx"

parser_info::parser_info(istream_wrapper &_stream)
  : stream(_stream), buf()
{
}

mp_token_t
parser_info::peek(void)
{
  mp_token_t t = get();
  unget(t); // Unget guarantee succeeds
  return t;
}

mp_token_t
parser_info::get(void)
{
  if (buf.empty()) return next_token(stream);
  mp_token_t tok = buf.back();
  buf.pop_back();
  return tok;
}

void
parser_info::unget(mp_token_t t)
{
  buf.push_back(t);
}

static
std::string
gen_err_msg(mp_token_t tok, const std::string &msg)
{
  std::stringstream sstr;
  sstr << "Found " << to_string(tok) << " when parsing";
  if (!msg.empty()) sstr << ": " << msg;
  return sstr.str();
}

parser_error::parser_error(mp_token_t tok, const std::string &msg)
  : std::runtime_error(gen_err_msg(tok, msg))
{
}

mp_token_t
consume(parser_info &src)
{
  mp_token_t tok = src.get();
  if (tok.type == mp_token_t::S_ERR) throw parser_error(tok);
  return tok;
}

mp_token_t
one_of(parser_info &src, std::vector<mp_token_t::tok_type> types)
{
  mp_token_t tok = consume(src);
  auto res = std::find_if(std::begin(types), std::end(types),
			  [&tok](mp_token_t::tok_type type){
			    return type == tok.type;
			  });
  if (res != std::end(types)) return tok;

  // Construct error msg
  std::stringstream sstr;
  sstr << "Expected ";
  if (types.size() == 1) sstr << to_string(types[0]);
  else
    {
      sstr << "one of";
      for (const auto &el : types) sstr << ' ' << to_string(el);
    }
  throw parser_error(tok, sstr.str());
}

bool
optional(parser_info &src, std::vector<mp_token_t::tok_type> types)
{
  mp_token_t tok; // tok is discarded (used when you dont care about the token)
  return optional(src, types, tok);
}

bool
optional(parser_info &src, std::vector<mp_token_t::tok_type> types,
	 mp_token_t &ref)
{
  mp_token_t tok = consume(src);
  auto res = std::find_if(std::begin(types), std::end(types),
			  [&tok](mp_token_t::tok_type type){
			    return type == tok.type;
			  });
  if (res != std::end(types))
    {
      ref = tok;
      return true;
    }
  src.unget(tok);
  return false;
}

std::unique_ptr<syntree::expressions>
parse(parser_info &src)
{
  auto tree = parse_expressions(src);
  one_of(src, { mp_token_t::S_EOF });
  return tree;
}

std::unique_ptr<syntree::expressions>
parse_expressions (parser_info &src)
{
  // = expression
  // | expression ';' expressions

  auto tree = std::unique_ptr<syntree::expressions>(new syntree::expressions);
  while (true)
    {
      tree->push_back({ parse_expression(src) });
      if (!optional(src, { mp_token_t::Y_SEMI })) break;
    }
  return tree;
}

std::unique_ptr<syntree::expressions>
parse_argument_list (parser_info &src)
{
  // = expression
  // | expression ',' argument_list

  auto tree = std::unique_ptr<syntree::expressions>(new syntree::expressions);
  while (true)
    {
      tree->push_back({ parse_expression(src) });
      if (!optional(src, { mp_token_t::Y_COMMA })) break;
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_expression(parser_info &src)
{
  // = IDENT '=' lazy
  // | IDENT '<-' lazy
  // | lazy

  mp_token_t ident;
  if (optional(src, { mp_token_t::L_IDENT }, ident))
    {
      mp_token_t op;
      if (optional(src, { mp_token_t::Y_DECL, mp_token_t::Y_SET }, op))
	{
	  return std::unique_ptr<syntree::binop>(new syntree::binop(op, std::shared_ptr<syntree::ident>(new syntree::ident(ident)), { parse_expression(src) }));
	}

      // unget ident, its part of another expression
      src.unget(ident);
    }
  return parse_lazy(src);
}

std::unique_ptr<syntree::ast>
parse_lazy (parser_info &src)
{
  // = 'lazy' or
  // | or

  mp_token_t tok;
  if (optional(src, { mp_token_t::K_LAZY }, tok))
    {
      return std::unique_ptr<syntree::prefix>(new syntree::prefix(tok, { parse_or(src) }));
    }
  return parse_or(src);
}

std::unique_ptr<syntree::ast>
parse_or (parser_info &src)
{
  // = or 'or' and
  // | and

  auto lhs = parse_and(src);
  mp_token_t tok;
  while (optional(src, { mp_token_t::K_OR }, tok))
    {
      lhs = std::unique_ptr<syntree::binop>(new syntree::binop(tok, { std::move(lhs) }, { parse_and(src) }));
    }
  return lhs;
}

std::unique_ptr<syntree::ast>
parse_and (parser_info &src)
{
  // = and 'and' rel_like
  // | rel_like

  auto lhs = parse_rel_like(src);
  mp_token_t tok;
  while (optional(src, { mp_token_t::K_AND }, tok))
    {
      lhs = std::unique_ptr<syntree::binop>(new syntree::binop(tok, { std::move(lhs) }, { parse_rel_like(src) }));
    }
  return lhs;
}

std::unique_ptr<syntree::ast>
parse_rel_like (parser_info &src)
{
  // = cons '<' cons
  // | cons '>' cons
  // | cons '<=' cons
  // | cons '>=' cons
  // | cons '==' cons
  // | cons '/=' cons
  // | cons

  auto lhs = parse_cons(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_LT, mp_token_t::Y_GT, mp_token_t::Y_LE, mp_token_t::Y_GE, mp_token_t::Y_EQL, mp_token_t::Y_NEQ }, tok))
    {
      return std::unique_ptr<syntree::binop>(new syntree::binop(tok, { std::move(lhs) }, { parse_cons(src) }));
    }
  return lhs;
}

std::unique_ptr<syntree::ast>
parse_cons (parser_info &src)
{
  // = add_like
  // | add_like ':' cons

  std::unique_ptr<syntree::ast> tree = parse_add_like(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_COLON }, tok))
    {
      return std::unique_ptr<syntree::binop>(new syntree::binop(tok, { parse_cons(src) }, { std::move(tree) }));
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_add_like (parser_info &src)
{
  // = add_like '+' mul_like
  // | add_like '-' mul_like
  // | mul_like

  auto tree = parse_mul_like(src);
  mp_token_t tok;
  while (optional(src, { mp_token_t::Y_ADD, mp_token_t::Y_SUB }, tok))
    {
      tree = std::unique_ptr<syntree::binop>(new syntree::binop(tok, { std::move(tree) }, { parse_mul_like(src) }));
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_mul_like (parser_info &src)
{
  // = mul_like '*' prefix
  // | mul_like '/' prefix
  // | mul_like 'mod' prefix
  // | prefix

  auto tree = parse_prefix(src);
  mp_token_t tok;
  while (optional(src, { mp_token_t::Y_MUL, mp_token_t::Y_DIV, mp_token_t::K_MOD }, tok))
    {
      tree = std::unique_ptr<syntree::binop>(new syntree::binop(tok, { std::move(tree) }, { parse_prefix(src) }));
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_prefix (parser_info &src)
{
  // = '+' exponent
  // | '-' exponent
  // | exponent

  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_ADD, mp_token_t::Y_SUB }, tok))
    {
      return std::unique_ptr<syntree::prefix>(new syntree::prefix(tok, { parse_exponent(src) }));
    }
  return parse_exponent(src);
}

std::unique_ptr<syntree::ast>
parse_exponent (parser_info &src)
{
  // = postfix '^' prefix
  // | postfix

  auto tree = parse_postfix(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_POW }, tok))
    {
      return std::unique_ptr<syntree::binop>(new syntree::binop(tok, { std::move(tree) }, { parse_prefix(src) }));
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_postfix (parser_info &src)
{
  // = fapply '%'
  // | fapply

  auto tree = parse_fapply(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_PERCENT }, tok))
    {
      return std::unique_ptr<syntree::postfix>(new syntree::postfix(tok, { std::move(tree) }));
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_fapply (parser_info &src)
{
  // = parse_fapply '(' argument_list ')'
  // | parse_fapply '(' ')'
  // | fcompose

  auto tree = parse_fcompose(src);
  while (optional(src, { mp_token_t::P_LPAREN }))
    {
      if (optional(src, { mp_token_t::P_RPAREN }))
	{
	  return std::unique_ptr<syntree::fapply>(new syntree::fapply({ std::move(tree) }, nullptr));
	}
      auto params = parse_argument_list(src);
      one_of(src, { mp_token_t::P_RPAREN });
      // f(a, b, c) becomes f(a)(b)(c)
      for (size_t i = 0; i < params->size(); ++i)
	{
	  auto tmp = std::unique_ptr<syntree::fapply>(new syntree::fapply({ std::move(tree) }, (*params)[i]));
	  tree = std::move(tmp);
	}
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_fcompose (parser_info &src)
{
  // = qexpr '.' fcompose
  // | qexpr

  std::unique_ptr<syntree::ast> tree = parse_qexpr(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_PERIOD }, tok))
    {
      return std::unique_ptr<syntree::binop>(new syntree::binop(tok, { std::move(tree) }, { parse_fcompose(src) }));
    }
  return tree;
}

std::unique_ptr<syntree::ast>
parse_qexpr (parser_info &src)
{
  // = '&' qexpr
  // | do_end

  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_QEXPR }, tok))
    {
      return std::unique_ptr<syntree::prefix>(new syntree::prefix(tok, { parse_qexpr(src) }));
    }
  return parse_do_end(src);
}

std::unique_ptr<syntree::ast>
parse_do_end (parser_info &src)
{
  // = 'do' 'end' #=> Parser error
  // | 'do' expressions 'end'
  // | if_expr

  if (optional(src, { mp_token_t::K_DO }))
    {
      mp_token_t tok;
      if (optional(src, { mp_token_t::K_END }, tok))
	{
	  throw parser_error(tok, "Empty do-end is an illegal construct");
	}
      auto exprs = parse_expressions(src);
      one_of(src, { mp_token_t::K_END });
      return exprs;
    }
  return parse_if_expr(src);
}

std::unique_ptr<syntree::ast>
parse_if_expr (parser_info &src)
{
  // = '{' '}' #=> Parser Error
  // | '{' <<cases>> '}'
  // | primitive
  //
  // <<cases>>
  // = expression 'else'
  // | expression 'if' predicate
  // | expression 'if' predicate ',' <<cases>>

  if (optional(src, { mp_token_t::P_LCURL }))
    {
      mp_token_t tok;
      if (optional(src, { mp_token_t::P_RCURL }, tok))
	{
	  throw parser_error(tok, "{ } without cases is an illegal construct");
	}

      auto tree = std::unique_ptr<syntree::cond_expr>(new syntree::cond_expr);

      while (true)
	{
	  auto expr = parse_expressions(src);
	  mp_token_t tok = one_of(src, { mp_token_t::K_ELSE, mp_token_t::K_IF });
	  if (tok.type == mp_token_t::K_IF)
	    {
	      tree->push_if_node(std::shared_ptr<syntree::binop>(new syntree::binop(tok, { parse_expressions(src) }, { std::move(expr) })));

	      if (optional(src, { mp_token_t::Y_COMMA })) continue;
	      break;
	    }
	  // else clause
	  // CONSIDER MAKING IF AND ELSE THEIR OWN CLASSES
	  tree->set_else_node(std::shared_ptr<syntree::postfix>(new syntree::postfix(tok, { std::move(expr) })));
	  break;
	}
      one_of(src, { mp_token_t::P_RCURL });
      return tree;
    }
  return parse_primitive(src);
}

static const std::vector<mp_token_t::tok_type> impl_produce
{
  mp_token_t::Y_YIELD, mp_token_t::Y_MACRO
};

std::unique_ptr<syntree::ast>
parse_primitive (parser_info &src)
{
  // = NUMBER
  // | ATOM
  // | IDENT <<produce>> expression
  // | IDENT
  // | '(' ')' <<produce>> expression
  // | '(' <<params>> ')' <<produce>> expression
  // | '(' expression ')'
  // | '[' argument_list ']'
  // | '[' ']'
  //
  // <<produce>>
  // = '->'
  // | '=>'
  //
  // <<params>>
  // = IDENT
  // | IDENT ',' <<params>>
  if (optional(src, { mp_token_t::P_LSQUARE }))
    {
      if (optional(src, { mp_token_t::P_RSQUARE })) return std::unique_ptr<syntree::array>(new syntree::array);
      auto datum = parse_argument_list(src);
      
      mp_token_t tok = one_of(src, { mp_token_t::P_RSQUARE });
      auto tree = std::unique_ptr<syntree::array>(new syntree::array);
      for (size_t i = datum->size(); i > 0; --i)
	{
	  tree->push_front((*datum)[i - 1]);
	}
      return tree;
    }
  if (optional(src, { mp_token_t::P_LPAREN }))
    {
      if (optional(src, { mp_token_t::P_RPAREN }))
	{
	  /*
	   * To the evaluator, an <ident> being sent the <<produce>>
	   * operator construct a function (or macro). To the runtime,
	   * if the parameter is an empty string, it is a function taking
	   * no parameters.
	   *
	   * We create an <ident> with <tok.text> of <"">
	   */
	  mp_token_t op = one_of(src, impl_produce);
	  std::shared_ptr<syntree::ident> fake_param(new syntree::ident({
		.type = mp_token_t::L_IDENT, .line_num = op.line_num, .col_num = op.col_num, .text = "" }));
	  return std::unique_ptr<syntree::binop>(new syntree::binop(op, fake_param, parse_expression(src)));
	}
      // Assume it is a function
      mp_token_t p;
      if (optional(src, { mp_token_t::L_IDENT }, p))
	{
	  if (optional(src, { mp_token_t::Y_COMMA }))
	    {
	      // Has to be a function in form of (a, b) -> <<expr>>
	      // => a -> (b -> <<expr>>)
	      std::vector<mp_token_t> ps;
	      ps.push_back(p);
	      do ps.push_back(one_of(src, { mp_token_t::L_IDENT }));
	      while (optional(src, { mp_token_t::Y_COMMA }));
	      one_of(src, { mp_token_t::P_RPAREN });
	      mp_token_t op = one_of(src, impl_produce);

	      auto tree = parse_expression(src);
	      for (size_t i = ps.size(); i > 0; --i)
		{
		  tree = std::unique_ptr<syntree::binop>(new syntree::binop(op, std::shared_ptr<syntree::ident>(new syntree::ident(ps[i - 1])), { std::move(tree) }));
		}
	      return tree;
	    }
	  if (optional(src, { mp_token_t::P_RPAREN }))
	    {
	      mp_token_t op;
	      if (optional(src, impl_produce, op))
		{
		  // Has to be a function in form of (a) -> <<expr>>
		  return std::unique_ptr<syntree::binop>(new syntree::binop(op, std::shared_ptr<syntree::ident>(new syntree::ident(p)), { parse_expression(src) }));
		}
	      // Its an expression ( x )
	      // => return x as syntree
	      return std::unique_ptr<syntree::ident>(new syntree::ident(p));
	    }
	  // Unshift token: its an expression, not a function
	  src.unget(p);
	}

      auto body = parse_expression(src);
      one_of(src, { mp_token_t::P_RPAREN });
      return body;
    }
  mp_token_t tok;
  if (optional(src, { mp_token_t::L_IDENT }, tok))
    {
      mp_token_t op;
      if (optional(src, impl_produce, op))
	{
	  return std::unique_ptr<syntree::binop>(new syntree::binop(op, std::shared_ptr<syntree::ident>(new syntree::ident(tok)), { parse_expression(src) }));
	}
      return std::unique_ptr<syntree::ident>(new syntree::ident(tok));
    }
  if (optional(src, { mp_token_t::L_NUMBER }, tok))
    {
      return std::unique_ptr<syntree::num>(new syntree::num(tok));
    }
  return std::unique_ptr<syntree::atom>(new syntree::atom(one_of(src, { mp_token_t::L_ATOM })));
}
