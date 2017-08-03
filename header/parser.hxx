#ifndef _MP_PARSER_HXX__
#define _MP_PARSER_HXX__

#include "lexer.hxx"

#include "rt_utils.h"

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
  parser_info &operator=(parser_info &&mref);

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
mp_token_t one_of(parser_info &src, std::vector<mp_token_t::tok_type> types);

// Reference is not modified (and token is unget-ed) if type is incorrect
bool optional(parser_info &src, std::vector<mp_token_t::tok_type> types);
bool optional(parser_info &src, std::vector<mp_token_t::tok_type> types,
	      mp_token_t &ref);

rt_data_t *parse (parser_info &src);

std::vector<rt_data_t*> parse_expressions (parser_info &src);
std::vector<rt_data_t*> parse_argument_list (parser_info &src);

rt_data_t *parse_expression (parser_info &src);
rt_data_t *parse_lazy (parser_info &src);
rt_data_t *parse_or (parser_info &src);
rt_data_t *parse_and (parser_info &src);
rt_data_t *parse_rel_like (parser_info &src);
rt_data_t *parse_cons (parser_info &src);
rt_data_t *parse_add_like (parser_info &src);
rt_data_t *parse_mul_like (parser_info &src);
rt_data_t *parse_prefix (parser_info &src);
rt_data_t *parse_exponent (parser_info &src);
rt_data_t *parse_postfix (parser_info &src);
rt_data_t *parse_fapply (parser_info &src);
rt_data_t *parse_fcompose (parser_info &src);
rt_data_t *parse_qexpr (parser_info &src);
rt_data_t *parse_do_end (parser_info &src);
rt_data_t *parse_if_expr (parser_info &src);
rt_data_t *parse_primitive (parser_info &src);

#endif /* !_MP_PARSER_HXX__ */
