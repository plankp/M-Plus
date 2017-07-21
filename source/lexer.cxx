#include "lexer.hxx"

istream_wrapper::istream_wrapper(std::istream &_src)
  : stream(_src), line_num(0), col_num(0)
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
  return (mp_token_t)
    {
      .type = mp_token_t::S_EOF,
	.line_num = src.get_line_num(),
	.col_num = src.get_col_num(),
	.text = ""
	};
}

static
mp_token_t
create_err_token (istream_wrapper &src, char ch)
{
  return (mp_token_t)
    {
      .type = mp_token_t::S_ERR,
	.line_num = src.get_line_num(),
	.col_num = src.get_col_num(),
	.text = std::string(1, ch)
	};
}

mp_token_t
next_token (istream_wrapper &src)
{
  while (true)
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
	  while (!is_comment_end(src.get())) {
	    ; // Just continue consuming
	  }
	  break;
	case '{':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::P_LCURL,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "{"
		};
	case '}':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::P_RCURL,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "}"
		};
	case '[':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::P_LSQUARE,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "["
		};
	case ']':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::P_RSQUARE,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "]"
		};
	case '(':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::P_LPAREN,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "("
		};
	case ')':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::P_RPAREN,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = ")"
		};
	case '+':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_ADD,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "+"
		};
	case '-':
	  {
	    const int ch = src.get();
	    if (ch == '>')
	      return (mp_token_t)
		{
		  .type = mp_token_t::Y_YIELD,
		    .line_num = src.get_line_num(),
		    .col_num = src.get_col_num(),
		    .text = "->"
		    };

	    src.unget(ch);
	    return (mp_token_t)
	      {
		.type = mp_token_t::Y_SUB,
		  .line_num = src.get_line_num(),
		  .col_num = src.get_col_num(),
		  .text = "-"
		  };
	  }
	case '*':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_MUL,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "*"
		};
	case '/':
	  {
	    const int ch = src.get();
	    if (ch == '=')
	      return (mp_token_t)
		{
		  .type = mp_token_t::Y_NEQ,
		    .line_num = src.get_line_num(),
		    .col_num = src.get_col_num(),
		    .text = "/="
		    };

	    src.unget(ch);
	    return (mp_token_t)
	      {
		.type = mp_token_t::Y_DIV,
		  .line_num = src.get_line_num(),
		  .col_num = src.get_col_num(),
		  .text = "/"
		};
	  }
	case '^':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_POW,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "^"
		};
	case '%':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_PERCENT,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "%"
		};
	case '&':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_QEXPR,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "&"
		};
	case ':':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_COLON,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = ":"
		};
	case ';':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_SEMI,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = ";"
		};
	case ',':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_COMMA,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = ","
		};
	case '.':
	  return (mp_token_t)
	    {
	      .type = mp_token_t::Y_PERIOD,
		.line_num = src.get_line_num(),
		.col_num = src.get_col_num(),
		.text = "."
		};
	case '=':
	  {
	    const int ch = src.get();
	    if (ch == '=')
	      return (mp_token_t)
		{
		  .type = mp_token_t::Y_EQL,
		    .line_num = src.get_line_num(),
		    .col_num = src.get_col_num(),
		    .text = "=="
		    };

	    src.unget(ch);
	    return (mp_token_t)
	      {
		.type = mp_token_t::Y_DECL,
		  .line_num = src.get_line_num(),
		  .col_num = src.get_col_num(),
		  .text = "="
		  };
	  }
	case '>':
	  {
	    const int ch = src.get();
	    if (ch == '=')
	      return (mp_token_t)
		{
		  .type = mp_token_t::Y_GE,
		    .line_num = src.get_line_num(),
		    .col_num = src.get_col_num(),
		    .text = ">="
		    };

	    src.unget(ch);
	    return (mp_token_t)
	      {
		.type = mp_token_t::Y_GT,
		  .line_num = src.get_line_num(),
		  .col_num = src.get_col_num(),
		  .text = ">"
		  };
	  }
	case '<':
	  {
	    const int ch = src.get();
	    switch (ch)
	      {
	      case '-':
		return (mp_token_t)
		  {
		    .type = mp_token_t::Y_SET,
		      .line_num = src.get_line_num(),
		      .col_num = src.get_col_num(),
		      .text = "<-"
		      };
	      case '=':
		return (mp_token_t)
		  {
		    .type = mp_token_t::Y_LE,
		      .line_num = src.get_line_num(),
		      .col_num = src.get_col_num(),
		      .text = "<="
		      };
	      default:
		src.unget(ch);
		return (mp_token_t)
		  {
		    .type = mp_token_t::Y_LT,
		      .line_num = src.get_line_num(),
		      .col_num = src.get_col_num(),
		      .text = "<"
		      };
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
  while (true)
    {
      ch = src.get();
      if (is_ident(ch)) buf << static_cast<char>(ch);
      else break;
    }
  // ch now must be either ?, !, or something for another cycle to process
  if (ch == '?' || ch == '!') buf << static_cast<char>(ch);
  else src.unget(ch);
}

static const std::map<std::string, mp_token_t::tok_type> KWORDS
{
  { "mod", mp_token_t::K_MOD },
  { "if", mp_token_t::K_IF },
  { "else", mp_token_t::K_ELSE },
  { "and", mp_token_t::K_AND },
  { "or", mp_token_t::K_OR },
  { "do", mp_token_t::K_DO },
  { "end", mp_token_t::K_END },
  { "lazy", mp_token_t::K_LAZY }
};

mp_token_t
next_symbol (istream_wrapper &src)
{
  std::stringstream stream;
  int ch = src.get();
  if (ch == EOF) return create_eof_token(src);
  if (is_ident(ch))
    {
      stream << static_cast<char>(ch);
      next_opt_ident(src, stream);
      std::string s = stream.str();
      auto it = KWORDS.find(s);
      return (mp_token_t)
	{
	  .type = it == KWORDS.end() ? mp_token_t::L_IDENT : it->second,
	    .line_num = src.get_line_num(),
	    .col_num = src.get_col_num(),
	    .text = s
	    };
    }
  return create_err_token(src, ch);
}

static inline
void
next_zero_number (istream_wrapper &src, std::stringstream &sstr)
{
  int ch = src.get();
  bool (*pred)(int) = nullptr;
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
    do
      {
	sstr << static_cast<char>(ch);
	ch = src.get();
      }
    while (pred(ch));
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
      return (mp_token_t)
	{
	  .type = mp_token_t::L_NUMBER,
	    .line_num = src.get_line_num(),
	    .col_num = src.get_col_num(),
	    .text = sstr.str()
	    };
    }
  if (is_decimal(ch))
    {
      std::stringstream sstr;
      sstr << static_cast<char>(ch);
      next_raw_decimal(src, sstr);
      return (mp_token_t)
	{
	  .type = mp_token_t::L_NUMBER,
	    .line_num = src.get_line_num(),
	    .col_num = src.get_col_num(),
	    .text = sstr.str()
	    };
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
      return (mp_token_t)
	{
	  .type = mp_token_t::L_ATOM,
	    .line_num = src.get_line_num(),
	    .col_num = src.get_col_num(),
	    .text = sstr.str()
	    };
    }
  return create_err_token(src, ch);
}
