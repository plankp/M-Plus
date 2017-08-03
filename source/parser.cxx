#include "parser.hxx"

#include <stdio.h>

parser_info::parser_info(istream_wrapper &_stream)
  : stream(_stream), buf()
{
}

parser_info::parser_info(parser_info &&mref)
  : stream(mref.stream), buf(mref.buf)
{
}

parser_info &
parser_info::operator=(parser_info &&mref)
{
  using std::swap;

  swap(*this, mref);
  return *this;
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

rt_data_t *
parse(parser_info &src)
{
  auto tree = parse_expressions(src);
  one_of(src, { mp_token_t::S_EOF });

  if (tree.size() == 1) return tree[0];

  // assume its a do-end expression
  tree.insert(tree.begin(), from_atom("do"));
  auto ret = from_list(tree.size(), tree.data());
  for (size_t i = 0; i < tree.size(); ++i) dealloc(&tree[i]);
  return ret;
}

std::vector<rt_data_t*>
parse_expressions (parser_info &src)
{
  // = expression
  // | expression ';' expressions

  std::vector<rt_data_t*> vec;
  do vec.push_back(parse_expression(src));
  while (optional(src, { mp_token_t::Y_SEMI }));
  return vec;
}

std::vector<rt_data_t*>
parse_argument_list (parser_info &src)
{
  // = expression
  // | expression ',' argument_list

  std::vector<rt_data_t*> vec;
  do vec.push_back(parse_expression(src));
  while (optional(src, { mp_token_t::Y_COMMA }));
  return vec;
}

rt_data_t *
parse_expression(parser_info &src)
{
  // = IDENT '=' expression
  // | IDENT '<-' expression
  // | lazy

  mp_token_t ident;
  if (optional(src, { mp_token_t::L_IDENT }, ident))
    {
      mp_token_t op;
      if (optional(src, { mp_token_t::Y_DECL, mp_token_t::Y_SET }, op))
	{
	  return make_binary_expr(from_atom(op.text.c_str()),
				  from_atom(ident.text.c_str()),
				  parse_expression(src));
	}

      // unget ident, its part of another expression
      src.unget(ident);
    }
  return parse_lazy(src);
}

rt_data_t *
parse_lazy (parser_info &src)
{
  // = 'lazy' or
  // | or

  mp_token_t tok;
  if (optional(src, { mp_token_t::K_LAZY }, tok))
    {
      return make_unary_expr(from_atom(tok.text.c_str()),
			     parse_or(src));
    }
  return parse_or(src);
}

rt_data_t *
parse_or (parser_info &src)
{
  // = or 'or' and
  // | and

  auto lhs = parse_and(src);
  mp_token_t tok;
  while (optional(src, { mp_token_t::K_OR }, tok))
    {
      lhs = make_binary_expr(from_atom(tok.text.c_str()),
			     lhs,
			     parse_and(src));
    }
  return lhs;
}

rt_data_t *
parse_and (parser_info &src)
{
  // = and 'and' rel_like
  // | rel_like

  auto lhs = parse_rel_like(src);
  mp_token_t tok;
  while (optional(src, { mp_token_t::K_AND }, tok))
    {
      lhs = make_binary_expr(from_atom(tok.text.c_str()),
			     lhs,
			     parse_rel_like(src));
    }
  return lhs;
}

rt_data_t *
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
      return make_binary_expr(from_atom(tok.text.c_str()),
			      lhs,
			      parse_cons(src));
    }
  return lhs;
}

rt_data_t *
parse_cons (parser_info &src)
{
  // = add_like
  // | add_like ':' cons

  auto tree = parse_add_like(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_COLON }, tok))
    {
      return make_binary_expr(from_atom(tok.text.c_str()),
			      tree,
			      parse_cons(src));
    }
  return tree;
}

rt_data_t *
parse_add_like (parser_info &src)
{
  // = add_like '+' mul_like
  // | add_like '-' mul_like
  // | mul_like

  auto tree = parse_mul_like(src);
  mp_token_t tok;
  while (optional(src, { mp_token_t::Y_ADD, mp_token_t::Y_SUB }, tok))
    {
      tree = make_binary_expr(from_atom(tok.text.c_str()),
			      tree,
			      parse_mul_like(src));
    }
  return tree;
}

rt_data_t *
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
      tree = make_binary_expr(from_atom(tok.text.c_str()),
			      tree,
			      parse_prefix(src));
    }
  return tree;
}

rt_data_t *
parse_prefix (parser_info &src)
{
  // = '+' exponent
  // | '-' exponent
  // | exponent

  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_ADD, mp_token_t::Y_SUB }, tok))
    {
      return make_unary_expr(from_atom(tok.text.c_str()),
			     parse_exponent(src));
    }
  return parse_exponent(src);
}

rt_data_t *
parse_exponent (parser_info &src)
{
  // = postfix '^' prefix
  // | postfix

  auto tree = parse_postfix(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_POW }, tok))
    {
      return make_binary_expr(from_atom(tok.text.c_str()),
			      tree,
			      parse_prefix(src));
    }
  return tree;
}

rt_data_t *
parse_postfix (parser_info &src)
{
  // = fapply '%'
  // | fapply

  auto tree = parse_fapply(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_PERCENT }, tok))
    {
      return make_unary_expr(from_atom(tok.text.c_str()),
			     tree);
    }
  return tree;
}

rt_data_t *
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
	  return make_nullary_expr(tree);
	}
      auto params = parse_argument_list(src);
      one_of(src, { mp_token_t::P_RPAREN });
      // f(a, b, c) becomes f(a)(b)(c)
      for (size_t i = 0; i < params.size(); ++i)
	{
	  tree = make_unary_expr(tree, params[i]);
	}
    }
  return tree;
}

rt_data_t *
parse_fcompose (parser_info &src)
{
  // = qexpr '.' fcompose
  // | qexpr

  auto tree = parse_qexpr(src);
  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_PERIOD }, tok))
    {
      return make_binary_expr(from_atom(tok.text.c_str()),
			      tree,
			      parse_fcompose(src));
    }
  return tree;
}

rt_data_t *
parse_qexpr (parser_info &src)
{
  // = '&' qexpr
  // | do_end

  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_QEXPR }, tok))
    {
      return make_unary_expr(from_atom(tok.text.c_str()),
			     parse_qexpr(src));
    }
  return parse_do_end(src);
}

rt_data_t *
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
      exprs.insert(exprs.begin(), from_atom("do"));
      auto ret = from_list(exprs.size(), exprs.data());
      for (size_t i = 0; i < exprs.size(); ++i)
	{
	  dealloc(&exprs[i]);
	}
      return ret;
    }
  return parse_if_expr(src);
}

rt_data_t *
parse_if_expr (parser_info &src)
{
  // = '{' '}' #=> Parser Error
  // | '{' <<cases>> '}'
  // | primitive
  //
  // <<cases>>
  // = expression 'else'
  // | expression 'if' expression
  // | expression 'if' expression ',' <<cases>>

  if (optional(src, { mp_token_t::P_LCURL }))
    {
      mp_token_t tok;
      if (optional(src, { mp_token_t::P_RCURL }, tok))
	{
	  throw parser_error(tok, "{ } without cases is an illegal construct");
	}

      std::vector<rt_data_t*> tree;
      for (;;)
	{
	  auto expr = parse_expression(src);
	  mp_token_t tok = one_of(src, { mp_token_t::K_ELSE, mp_token_t::K_IF });
	  if (tok.type == mp_token_t::K_IF)
	    {
	      tree.push_back(make_unary_expr(parse_expression(src),
					     expr));
	      if (optional(src, { mp_token_t::Y_COMMA })) continue;
	      break;
	    }
	  // else clause
	  tree.push_back(make_unary_expr(from_atom(tok.text.c_str()),
					 expr));
	  break;
	}
      one_of(src, { mp_token_t::P_RCURL });
      tree.insert(tree.begin(), from_atom("cond"));
      auto ret = from_list(tree.size(), tree.data());
      for (size_t i = 0; i < tree.size(); ++i)
	{
	  dealloc(&tree[i]);
	}
      return ret;
    }
  return parse_primitive(src);
}

static const std::vector<mp_token_t::tok_type> impl_produce
{
  mp_token_t::Y_YIELD, mp_token_t::Y_MACRO
};

static
long long
conv_ll(const std::string& text)
{
  if (text[0] == '0')
    {
      // Either its zero or it has a different base
      if (text.size() == 1) return 0;
      switch (text[1])
	{
	case 'b': return std::stoll(text.substr(2), nullptr, 2);
	case 'c': return std::stoll(text.substr(2), nullptr, 8);
	case 'd': return std::stoll(text.substr(2), nullptr, 10);
	case 'x': return std::stoll(text.substr(2), nullptr, 16);
	default:
	  throw std::invalid_argument("Unknown number literal: " + text);
	}
    }
  return std::stoll(text);
}

rt_data_t *
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
      if (optional(src, { mp_token_t::P_RSQUARE })) return alloc_array(0);
      auto datum = parse_argument_list(src);
      
      mp_token_t tok = one_of(src, { mp_token_t::P_RSQUARE });

      // Do not convert this into a from_array expression:
      // the content will not be evaluated (which is bad)
      auto tree = alloc_array(0);
      for (size_t i = datum.size(); i > 0; --i)
	{
	  tree = make_binary_expr(from_atom(":"),
				  datum[i - 1],
				  tree);
	}
      return tree;
    }
  if (optional(src, { mp_token_t::P_LPAREN }))
    {
      if (optional(src, { mp_token_t::P_RPAREN }))
	{
	  mp_token_t op = one_of(src, impl_produce);
	  return make_binary_expr(from_atom(op.text.c_str()),
				  alloc_list(0),
				  parse_expression(src));
	}
      // Assume it is a function
      mp_token_t p;
      if (optional(src, { mp_token_t::L_IDENT }, p))
	{
	  if (optional(src, { mp_token_t::Y_COMMA }))
	    {
	      // Has to be a function in form of (a, b) -> <<expr>>
	      // => (-> (a b) <<expr>>)
	      std::vector<mp_token_t> ps;
	      ps.push_back(p);
	      do ps.push_back(one_of(src, { mp_token_t::L_IDENT }));
	      while (optional(src, { mp_token_t::Y_COMMA }));
	      one_of(src, { mp_token_t::P_RPAREN });
	      mp_token_t op = one_of(src, impl_produce);

	      auto tree = parse_expression(src);
	      auto params = alloc_list(ps.size());
	      for (size_t i = 0; i < ps.size(); ++i)
		{
		  params->_list.list[i] = from_atom(ps[i].text.c_str());
		}
	      return make_binary_expr(from_atom(op.text.c_str()), params, tree);
	    }
	  if (optional(src, { mp_token_t::P_RPAREN }))
	    {
	      mp_token_t op;
	      if (optional(src, impl_produce, op))
		{
		  // Has to be a function in form of (a) -> <<expr>>
		  return make_binary_expr(from_atom(op.text.c_str()),
					  make_nullary_expr(from_atom(p.text.c_str())),
					  parse_expression(src));
		}
	      // Its an expression ( x )
	      // => return x as syntree
	      return from_atom(p.text.c_str());
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
	  return make_binary_expr(from_atom(op.text.c_str()),
				  make_nullary_expr(from_atom(tok.text.c_str())),
				  parse_expression(src));
	}
      return from_atom(tok.text.c_str());
    }
  if (optional(src, { mp_token_t::L_NUMBER }, tok))
    {
      if (tok.text.find('.') == std::string::npos)
	{
	  return from_int64(conv_ll(tok.text));
	}
      return from_double(std::stod(tok.text));
    }
  return from_string(one_of(src, { mp_token_t::L_ATOM }).text.c_str());
}
