#ifndef _MP_TOKEN_HXX__
#define _MP_TOKEN_HXX__

#include <string>
#include <sstream>

/*
 * Naming conventions:
 *  S_ - Special (errors, <<eof>>, or invisible characters)
 *  L_ - Literal (actual representation is user dependent)
 *  K_ - Keyword
 *  P_ - Pair (symbols that have a begin and end: brackets)
 *  Y_ - Symbol (encompases everything else)
 */
#define MP_TOKEN_TYPES				\
  X(S_EOF)					\
  X(S_ERR)					\
  X(L_ATOM)					\
  X(L_NUMBER)					\
  X(L_IDENT)					\
  X(P_LCURL)					\
  X(P_RCURL)					\
  X(P_LSQUARE)					\
  X(P_RSQUARE)					\
  X(P_LPAREN)					\
  X(P_RPAREN)					\
  X(K_MOD)					\
  X(K_IF)					\
  X(K_ELSE)					\
  X(K_AND)					\
  X(K_OR)					\
  X(K_DO)					\
  X(K_END)					\
  X(K_LAZY)					\
  X(Y_ADD)					\
  X(Y_SUB)					\
  X(Y_MUL)					\
  X(Y_DIV)					\
  X(Y_POW)					\
  X(Y_PERCENT)					\
  X(Y_QEXPR)					\
  X(Y_COLON)					\
  X(Y_SEMI)					\
  X(Y_COMMA)					\
  X(Y_PERIOD)					\
  X(Y_SET)					\
  X(Y_DECL)					\
  X(Y_YIELD)					\
  X(Y_MACRO)					\
  X(Y_LT)					\
  X(Y_GT)					\
  X(Y_LE)					\
  X(Y_GE)					\
  X(Y_EQL)					\
  X(Y_NEQ)

struct mp_token_t
{
#define X(n) n,
  enum tok_type { /* Macro expansion here */ MP_TOKEN_TYPES } type;
#undef X
  size_t line_num;
  size_t col_num;
  std::string text;
};

std::string to_string (const mp_token_t::tok_type &ref);
std::string to_string (const mp_token_t &ref);

#endif /* !_MP_TOKEN_HXX__ */
