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
 *
 * X(TOKEN NAME, PREC, RIGHT PREC, NEXT PREC, IS BINARY)
 *   If any of the fields are not applicable, fill in -1 (IS BINARY is 0)
 * XN(TOKEN NAME)
 *   The other fields auto fill to -1
 */
#define MP_TOKEN_TYPES				\
  X(K_OR,      10, 11, 10, 1)			\
  X(K_AND,     20, 21, 20, 1)			\
  X(Y_LT,      30, 40, 29, 1)			\
  X(Y_GT,      30, 40, 29, 1)			\
  X(Y_LE,      30, 40, 29, 1)			\
  X(Y_GE,      30, 40, 29, 1)			\
  X(Y_EQL,     30, 40, 29, 1)			\
  X(Y_NEQ,     30, 40, 29, 1)			\
  X(Y_COLON,   40, 40, 39, 1)			\
  X(Y_ADD,     50, 51, 50, 1)			\
  X(Y_SUB,     50, 51, 50, 1)			\
  X(Y_MUL,     60, 61, 60, 1)			\
  X(Y_DIV,     60, 61, 60, 1)			\
  X(K_MOD,     60, 61, 60, 1)			\
  X(Y_POW,     70, 70, 69, 1)			\
  X(Y_PERCENT, 80, -1, 80, 0)			\
  X(Y_PERIOD,  90, 90, 89, 1)			\
  XN(S_EOF)					\
  XN(S_ERR)					\
  XN(L_ATOM)					\
  XN(L_NUMBER)					\
  XN(L_IDENT)					\
  XN(P_LCURL)					\
  XN(P_RCURL)					\
  XN(P_LSQUARE)					\
  XN(P_RSQUARE)					\
  XN(P_LPAREN)					\
  XN(P_RPAREN)					\
  XN(K_IF)					\
  XN(K_ELSE)					\
  XN(K_DO)					\
  XN(K_END)					\
  XN(K_LAZY)					\
  XN(K_TRY)					\
  XN(K_CATCH)					\
  XN(Y_QEXPR)					\
  XN(Y_SEMI)					\
  XN(Y_COMMA)					\
  XN(Y_SET)					\
  XN(Y_DECL)					\
  XN(Y_YIELD)

struct mp_token_t
{
#define X(n, a, b, c, d) n,
#define XN(n) n,
  enum tok_type { /* Macro expansion here */ MP_TOKEN_TYPES } type;
#undef XN
#undef X
  size_t line_num;
  size_t col_num;
  std::string text;
};

std::string to_string (const mp_token_t::tok_type &ref);
std::string to_string (const mp_token_t &ref);

#endif /* !_MP_TOKEN_HXX__ */
