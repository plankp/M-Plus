#ifndef _MP_PARSER_HXX__
#define _MP_PARSER_HXX__

#include "lexer.hxx"

#include "rt_utils.h"

#include <limits>
#include <memory>
#include <vector>
#include <utility>
#include <algorithm>
#include <stdexcept>

class parser_info
{
private:
  istream_wrapper &stream;
  std::vector<mp_token_t> buf;

public:
  parser_info() = delete;
  parser_info(istream_wrapper &stream);
  parser_info(const parser_info &ref) = delete;
  parser_info(parser_info &&mref);
  ~parser_info() = default;

  parser_info &operator=(const parser_info &ref) = delete;
  parser_info &operator=(parser_info &&mref) = delete;

  mp_token_t peek(void);
  mp_token_t get(void);
  void unget(mp_token_t t);
};

class parser_error : public std::runtime_error
{
public:
  parser_error() = delete;
  parser_error(mp_token_t tok, const std::string &msg = "");
};

// Throws error if token has type S_ERR
mp_token_t consume(parser_info &src);

// $type must not be S_ERR (its already handled)
mp_token_t one_of(parser_info &src, mp_token_t::tok_type type);
mp_token_t one_of(parser_info &src,
		  const std::vector<mp_token_t::tok_type> &types);

// Reference is not modified (and token is unget-ed) if type is incorrect
bool optional(parser_info &src, mp_token_t::tok_type type);
bool optional(parser_info &src, mp_token_t::tok_type type, mp_token_t &ref);

rt_data_t *parse (parser_info &src);

std::vector<rt_data_t*> parse_expressions (parser_info &src);
std::vector<rt_data_t*> parse_argument_list (parser_info &src);

rt_data_t *parse_expression (parser_info &src);
rt_data_t *parse_head (parser_info &src);
rt_data_t *parse_subexpr (parser_info &src, int min_prec);

#endif /* !_MP_PARSER_HXX__ */
