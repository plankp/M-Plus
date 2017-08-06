#include "lexer.hxx"

istream_wrapper::istream_wrapper(std::istream &_src)
  : stream(_src), line_num(0), col_num(0)
{
}

istream_wrapper::istream_wrapper(istream_wrapper &&mref)
  : stream(mref.stream), line_num(mref.line_num), col_num(mref.col_num)
{
}

int
istream_wrapper::peek(void)
{
  return stream.peek();
}

int
istream_wrapper::get(void)
{
  int ch = stream.get();
  switch (ch)
    {
    case '\n': ++line_num; col_num = 0; break;
    case '\r': col_num = 0; break;
    case EOF: break;
    default: ++col_num; break;
    }
  return ch;
}

bool
istream_wrapper::unget(char ch)
{
  if (stream.putback(ch))
    {
      switch (ch)
	{
	case '\n': --line_num; break;
	default: --col_num; break;
	}
      return true;
    }
  return false;
}

size_t
istream_wrapper::get_line_num(void) const
{
  return line_num;
}

size_t
istream_wrapper::get_col_num(void) const
{
  return col_num;
}

static
mp_token_t
create_eof_token (istream_wrapper &src)
{
  return { mp_token_t::S_EOF, src.get_line_num(), src.get_col_num(),
      "" };
}

static
mp_token_t
create_err_token (istream_wrapper &src, char ch)
{
  return { mp_token_t::S_ERR, src.get_line_num(), src.get_col_num(),
      std::string(1, ch) };
}

mp_token_t
next_token (istream_wrapper &src)
{
  for (;;)
    {
      const int c = src.get();
      switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	  break;
	case '#':
	  while (not is_comment_end(src.get())) {
	    ; // Just continue consuming
	  }
	  break;
	case '{':
	  return { mp_token_t::P_LCURL,
	      src.get_line_num(), src.get_col_num(), "{" };
	case '}':
	  return { mp_token_t::P_RCURL,
	      src.get_line_num(), src.get_col_num(), "}" };
	case '[':
	  return { mp_token_t::P_LSQUARE,
	      src.get_line_num(), src.get_col_num(), "[" };
	case ']':
	  return { mp_token_t::P_RSQUARE,
	      src.get_line_num(), src.get_col_num(), "]" };
	case '(':
	  return { mp_token_t::P_LPAREN,
	      src.get_line_num(), src.get_col_num(), "(" };
	case ')':
	  return { mp_token_t::P_RPAREN,
	      src.get_line_num(), src.get_col_num(), ")" };
	case '+':
	  return { mp_token_t::Y_ADD,
	      src.get_line_num(), src.get_col_num(), "+" };
	case '-':
	  {
	    const int ch = src.get();
	    if (ch == '>')
	      return { mp_token_t::Y_YIELD,
		  src.get_line_num(), src.get_col_num(), "->" };

	    src.unget(ch);
	    return { mp_token_t::Y_SUB,
		src.get_line_num(), src.get_col_num(), "-" };
	  }
	case '*':
	  return { mp_token_t::Y_MUL,
	      src.get_line_num(), src.get_col_num(), "*" };
	case '/':
	  {
	    const int ch = src.get();
	    if (ch == '=')
	      return { mp_token_t::Y_NEQ,
		  src.get_line_num(), src.get_col_num(), "/=" };

	    src.unget(ch);
	    return { mp_token_t::Y_DIV,
		src.get_line_num(), src.get_col_num(), "/" };
	  }
	case '^':
	  return { mp_token_t::Y_POW,
	      src.get_line_num(), src.get_col_num(), "^" };
	case '%':
	  return { mp_token_t::Y_PERCENT,
	      src.get_line_num(), src.get_col_num(), "%" };
	case '&':
	  return { mp_token_t::Y_QEXPR,
	      src.get_line_num(), src.get_col_num(), "&" };
	case ':':
	  return { mp_token_t::Y_COLON,
	      src.get_line_num(), src.get_col_num(), ":" };
	case ';':
	  return { mp_token_t::Y_SEMI,
	      src.get_line_num(), src.get_col_num(), ";" };
	case ',':
	  return { mp_token_t::Y_COMMA,
	      src.get_line_num(), src.get_col_num(), "," };
	case '.':
	  return { mp_token_t::Y_PERIOD,
	      src.get_line_num(), src.get_col_num(), "." };
	case '=':
	  {
	    const int ch = src.get();
	    switch (ch)
	      {
	      case '=':
		return { mp_token_t::Y_EQL,
		    src.get_line_num(), src.get_col_num(), "==" };
	      default:
		src.unget(ch);
		return { mp_token_t::Y_DECL,
		    src.get_line_num(), src.get_col_num(), "=" };
	      }
	  }
	case '>':
	  {
	    const int ch = src.get();
	    if (ch == '=')
	      return { mp_token_t::Y_GE,
		  src.get_line_num(), src.get_col_num(), ">=" };

	    src.unget(ch);
	    return { mp_token_t::Y_GT, src.get_line_num(), src.get_col_num(),
		">" };
	  }
	case '<':
	  {
	    const int ch = src.get();
	    switch (ch)
	      {
	      case '-':
		return { mp_token_t::Y_SET,
		    src.get_line_num(), src.get_col_num(), "<-" };
	      case '=':
		return { mp_token_t::Y_LE,
		    src.get_line_num(), src.get_col_num(), "<=" };
	      default:
		src.unget(ch);
		return { mp_token_t::Y_LT,
		    src.get_line_num(), src.get_col_num(), "<" };
	      }
	  }
	case EOF:
	  return create_eof_token(src);
	case '@':
	  src.unget(c);
	  return next_atom(src);
	default:
	  if (is_decimal(c))
	    {
	      src.unget(c);
	      return next_number(src);
	    }
	  if (is_ident(c))
	    {
	      src.unget(c);
	      return next_symbol(src);
	    }

	  return create_err_token(src, c);
	}
    }
}

static
void
next_opt_ident (istream_wrapper &src, std::stringstream &buf)
{
  int ch;
  for (;;)
    {
      ch = src.get();
      if (is_ident(ch))
	{ buf << static_cast<char>(ch); }
      else break;
    }

  // ch now must be either ?, !, or something for another cycle to process
  if (ch == '?' || ch == '!')
    { buf << static_cast<char>(ch); }
  else src.unget(ch);
}

static std::map<std::string, mp_token_t::tok_type> KWORDS{
  { "mod", mp_token_t::K_MOD },
  { "if", mp_token_t::K_IF },
  { "else", mp_token_t::K_ELSE },
  { "and", mp_token_t::K_AND },
  { "or", mp_token_t::K_OR },
  { "do", mp_token_t::K_DO },
  { "end", mp_token_t::K_END },
  { "lazy", mp_token_t::K_LAZY },
  { "try", mp_token_t::K_TRY },
  { "catch", mp_token_t::K_CATCH }
};

mp_token_t
next_symbol (istream_wrapper &src)
{
  std::stringstream stream;
  int ch = src.get();
  if (ch == EOF)
    { return create_eof_token(src); }
  if (is_ident(ch))
    {
      stream << static_cast<char>(ch);
      next_opt_ident(src, stream);
      std::string s = stream.str();
      auto it = KWORDS.find(s);
      return { it == KWORDS.end() ? mp_token_t::L_IDENT : it->second,
	  src.get_line_num(), src.get_col_num(), s };
    }
  return create_err_token(src, ch);
}

static inline
void
next_zero_number (istream_wrapper &src, std::stringstream &sstr)
{
  int ch = src.get();
  bool (*pred)(int) = NULL;
  switch (ch)
    {
    case 'b':
      pred = &is_binary;
      break;
    case 'c':
      pred = &is_octal;
      break;
    case 'x':
      pred = &is_hexadecimal;
      break;
    case 'd':
    case '.':
      pred = &is_decimal;
      break;
    default:
      // Do nothing, the number was 0 itself
      break;
    }

  if (pred)
    { do
	{
	  sstr << static_cast<char>(ch);
	  ch = src.get();
	}
      while (pred(ch)); }
  src.unget(ch);
}

static inline
void
next_raw_decimal (istream_wrapper &src, std::stringstream &sstr)
{
  int ch = src.get();
  while (is_decimal(ch))
    {
      sstr << static_cast<char>(ch);
      ch = src.get();
    }

  // ch might be . in which case, consume floating point part too
  if (ch == '.')
    do
      {
	sstr << static_cast<char>(ch);
	ch = src.get();
      }
    while (is_decimal(ch));
  src.unget(ch);
}

mp_token_t
next_number (istream_wrapper &src)
{
  int ch = src.get();
  if (ch == EOF) return create_eof_token(src);
  if (ch == '0')
    {
      std::stringstream sstr;
      sstr << static_cast<char>(ch);
      next_zero_number(src, sstr);
      return { mp_token_t::L_NUMBER, src.get_line_num(), src.get_col_num(),
	  sstr.str() };
    }
  if (is_decimal(ch))
    {
      std::stringstream sstr;
      sstr << static_cast<char>(ch);
      next_raw_decimal(src, sstr);
      return { mp_token_t::L_NUMBER, src.get_line_num(), src.get_col_num(),
	  sstr.str() };
    }
  return create_err_token(src, ch);
}

mp_token_t
next_atom (istream_wrapper &src)
{
  int ch = src.get();
  if (ch == EOF) return create_eof_token(src);
  if (ch == '@')
    {
      std::stringstream sstr;
      sstr << static_cast<char>(ch);

      ch = src.get();
      if (ch == '"')
	{
	  // quoted-non-escaped-atom
	  do
	    {
	      sstr << static_cast<char>(ch);
	      ch = src.get();
	    }
	  while(ch != '"');
	  // The ending " is also part of the atom
	  sstr << static_cast<char>(ch);
	}
      else
	{
	  src.unget(ch);
	  next_opt_ident(src, sstr);
	}
      return { mp_token_t::L_ATOM, src.get_line_num(), src.get_col_num(),
	  sstr.str() };
    }
  return create_err_token(src, ch);
}
