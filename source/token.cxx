#include "token.hxx"

std::string
to_string (const mp_token_t::tok_type &ref)
{
#define X(n) case mp_token_t::n: return #n;
  switch (ref)
    { /* Macro expansion here! */ MP_TOKEN_TYPES }
#undef X
  // Returns an empty string by default
  return "";
}

std::string
to_string (const mp_token_t &ref)
{
  std::stringstream sstr;
  sstr << to_string(ref.type) << '(' << ref.text << ") "
       << "L:" << (ref.line_num + 1) << "," << ref.col_num;
  return sstr.str();
}
