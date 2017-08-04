#ifndef _MP_LEXER_HXX__
#define _MP_LEXER_HXX__

#include "token.hxx"

#include <map>
#include <cstdio>
#include <string>
#include <istream>
#include <sstream>
#include <utility>

class istream_wrapper
{
  std::istream &stream;
  size_t line_num;
  size_t col_num;

public:
  istream_wrapper() = delete;
  istream_wrapper(std::istream &stream);
  istream_wrapper(const istream_wrapper &ref) = delete;
  istream_wrapper(istream_wrapper &&mref);
  ~istream_wrapper() = default;

  istream_wrapper &operator=(const istream_wrapper &obj) = delete;
  istream_wrapper &operator=(istream_wrapper &&obj) = delete;

  int peek(void);
  int get(void);
  bool unget(char ch);

  size_t get_line_num(void) const;
  size_t get_col_num(void) const;
};

mp_token_t next_token (istream_wrapper &src);

mp_token_t next_number (istream_wrapper &src);
mp_token_t next_symbol (istream_wrapper &src);
mp_token_t next_atom (istream_wrapper &src);

inline
bool
is_comment_end (int ch)
{
  return ch == EOF || ch == '\n' || ch == '\r';
}

inline
bool
is_hexadecimal (int c)
{
  return (c >= '0' && c <= '9')
    || (c >= 'A' && c <= 'F')
    || (c >= 'a' && c <= 'f');
}

inline
bool
is_decimal (int c)
{
  return c >= '0' && c <= '9';
}

inline
bool
is_octal (int c)
{
  return c >= '0' && c <= '7';
}

inline
bool
is_binary (int c)
{
  return c == '0' || c == '1';
}

inline
bool
is_ident (int c)
{
  return  (c >= '0' && c <= '9')
    || (c >= 'A' && c <= 'Z')
    || (c >= 'a' && c <= 'z')
    || (c == '_')
    || (c == '$')
    || (c == '\'');
}

#endif /* !_MP_LEXER_HXX__ */
