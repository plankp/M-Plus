#include "parser.hxx"

parser_info::parser_info(istream_wrapper &_stream)
  : stream(_stream), buf()
{
}

parser_info::parser_info(parser_info &&mref)
  : stream(mref.stream), buf(mref.buf)
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
peek(parser_info &src)
{
  auto tok = consume(src);
  src.unget(tok);
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
  return parse_subexpr(src, 0);
}

// Parse table for subexpr
//
// No precedence should have negative value
static const std::map<mp_token_t::tok_type, std::array<int, 4>> table
{
  { mp_token_t::K_OR,      { 10, 11, 10, true } },
  { mp_token_t::K_AND,     { 20, 21, 20, true } },
  { mp_token_t::Y_LT,      { 30, 40, 29, true } },
  { mp_token_t::Y_GT,      { 30, 40, 29, true } },
  { mp_token_t::Y_LE,      { 30, 40, 29, true } },
  { mp_token_t::Y_GE,      { 30, 40, 29, true } },
  { mp_token_t::Y_EQL,     { 30, 40, 29, true } },
  { mp_token_t::Y_NEQ,     { 30, 40, 29, true } },
  { mp_token_t::Y_COLON,   { 40, 40, 39, true } },
  { mp_token_t::Y_ADD,     { 50, 51, 50, true } },
  { mp_token_t::Y_SUB,     { 50, 51, 50, true } },
  { mp_token_t::Y_MUL,     { 60, 61, 60, true } },
  { mp_token_t::Y_DIV,     { 60, 61, 60, true } },
  { mp_token_t::K_MOD,     { 60, 61, 60, true } },
  { mp_token_t::Y_POW,     { 70, 70, 69, true } },
  { mp_token_t::Y_PERCENT, { 80, -1, 80, false } },
  { mp_token_t::Y_PERIOD,  { 90, 90, 89, true } },
};

#define GET_PREC(t)						\
  ((table.find(t) == table.end()) ? -1 : (table.at(t)[0]))
#define GET_RIGHT_PREC(t)					\
  ((table.find(t) == table.end()) ? -1 : (table.at(t)[1]))
#define GET_NEXT_PREC(t)					\
  ((table.find(t) == table.end()) ? -1 : (table.at(t)[2]))
#define IS_BINARY(t)						\
  ((table.find(t) == table.end()) ? -1 : (table.at(t)[3]))

rt_data_t *
parse_head (parser_info &src)
{
  auto tok = consume(src);
  switch (tok.type)
    {
    case mp_token_t::K_LAZY:
      return make_unary_expr(from_atom(tok.text.c_str()),
			     parse_subexpr(src, GET_PREC(mp_token_t::K_OR)));
    case mp_token_t::Y_ADD:
    case mp_token_t::Y_SUB:
      return make_unary_expr(from_atom(tok.text.c_str()),
			     parse_subexpr(src, GET_PREC(mp_token_t::Y_MUL)));
    default:
      src.unget(tok);
      auto node = parse_qexpr(src);
      while (optional(src, { mp_token_t::P_LPAREN }))
	{
	  if (optional(src, { mp_token_t::P_RPAREN }))
	    {
	      return make_nullary_expr(node);
	    }
	  auto params = parse_argument_list(src);
	  one_of(src, { mp_token_t::P_RPAREN });
	  // f(a, b, c) becomes f(a)(b)(c)
	  for (size_t i = 0; i < params.size(); ++i)
	    {
	      node = make_unary_expr(node, params[i]);
	    }
	}
      return node;
    }
}

rt_data_t *
parse_subexpr (parser_info &src, int min_prec)
{
  if (min_prec < 0) return NULL;

  auto lhs = parse_head(src);
  auto max_prec = std::numeric_limits<int>::max();

  auto tok = peek(src);
  auto prec = GET_PREC(tok.type);

  while ((min_prec <= prec) && (prec <= max_prec))
    {
      tok = consume(src);
      if (IS_BINARY(tok.type))
	{
	  auto rhs = parse_subexpr(src, GET_RIGHT_PREC(tok.type));
	  lhs = make_binary_expr(from_atom(tok.text.c_str()), lhs, rhs);
	}
      else lhs = make_unary_expr(from_atom(tok.text.c_str()), lhs);
      max_prec = GET_NEXT_PREC(tok.type);

      tok = peek(src);
      prec = GET_PREC(tok.type);
    }

  return lhs;
}

#undef IS_BINARY
#undef GET_NEXT_PREC
#undef GET_RIGHT_PREC
#undef GET_PREC

rt_data_t *
parse_qexpr (parser_info &src)
{
  // = '&' qexpr
  // | try_catch

  mp_token_t tok;
  if (optional(src, { mp_token_t::Y_QEXPR }, tok))
    {
      return make_unary_expr(from_atom(tok.text.c_str()),
			     parse_qexpr(src));
    }
  return parse_try_catch(src);
}

rt_data_t *
parse_try_catch (parser_info &src)
{
  // = 'try' 'catch' #=> Parser error
  // | 'try' expression 'catch' expression
  // | 'try' expression 'catch' '->' IDENT expression
  // | do_end
  if (optional(src, { mp_token_t::K_TRY }))
    {
      mp_token_t tok;
      if (optional(src, { mp_token_t::K_CATCH }, tok))
	{
	  throw parser_error(tok, "Empty try-catch is an illegal construct");
	}
      auto try_act = parse_expression(src);
      one_of(src, { mp_token_t::K_CATCH });
      rt_data_t *catch_sto = nullptr;
      if (optional(src, { mp_token_t::Y_YIELD }))
	{
	  catch_sto = from_atom(one_of(src, { mp_token_t::L_IDENT }).text.c_str());
	}
      else catch_sto = alloc_list(0);
      return make_binary_expr(from_atom("try"),
			      try_act,
			      make_unary_expr(catch_sto, parse_expression(src)));
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

static
std::string
remove_clutter(const std::string &str)
{
  // Just in case, should never happen!
  if (str.length() == 0) return "";

  // Atom with length of 1 must be @
  if (str.length() == 1) return "";
  // Atom with lrngth of 2 must be @<char>
  if (str.length() == 2) return std::string(1, str[1]);
  // @"<str?>"
  if (str[1] == '"') return str.substr(2, str.size() - 3);
  // @<str>
  return str.substr(1);
}

rt_data_t *
parse_primitive (parser_info &src)
{
  // = NUMBER
  // | ATOM
  // | IDENT <<produce>> expression
  // | IDENT
  // | '(' ')'
  // | '(' ')' <<produce>> expression
  // | '(' <<params>> ')' <<produce>> expression
  // | '(' expression ')'
  // | '[' argument_list ']'
  // | '[' ']'
  //
  // <<produce>>
  // = '->'
  //
  // <<params>>
  // = IDENT
  // | IDENT ',' <<params>>
  #define impl_produce { mp_token_t::Y_YIELD }

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
	  mp_token_t op;
	  if (optional(src, impl_produce, op))
	    {
	      return make_binary_expr(from_atom(op.text.c_str()),
				      alloc_list(0),
				      parse_expression(src));
	    }
	  return alloc_list(0);
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
  return from_string(remove_clutter(one_of(src, { mp_token_t::L_ATOM }).text).c_str());
  
  #undef impl_produce
}
