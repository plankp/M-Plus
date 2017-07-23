#include "token_node.hxx"

namespace syntree
{
  token::token(mp_token_t _tok)
    : tok(_tok)
  {
  }

  token::token(const syntree::token &ref)
    : token(ref.tok)
  {
  }

  token::token(syntree::token &&ref)
    : token()
  {
    swap(*this, ref);
  }

  token::~token()
  {
  }

  syntree::token &
  token::operator=(syntree::token obj)
  {
    swap(*this, obj);
    return *this;
  }

  mp_token_t
  token::get_token(void)
  {
    return tok;
  }

  void
  token::accept(visitor &v)
  {
    v.visit_token(*this);
  }

  std::string
  token::type_name(void) const
  {
    return "token";
  }

  void
  swap(syntree::token &a, syntree::token &b)
  {
    using std::swap;

    swap(a.tok, b.tok);
  }
};
