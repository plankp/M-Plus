#include "num_node.hxx"

namespace syntree
{
  num::num(mp_token_t t)
    : tok(t)
  {
    if (t.type != mp_token_t::L_NUMBER)
      {
	throw std::invalid_argument("Expected L_NUMBER token");
      }
  }

  num::num(const syntree::num &ref)
    : tok(ref.tok)
  {
  }

  num::num(syntree::num &&mref)
    : num()
  {
    swap(*this, mref);
  }

  syntree::num &
  num::operator=(syntree::num obj)
  {
    swap(*this, obj);
    return *this;
  }

  mp_token_t
  num::get_token(void)
  {
    return tok;
  }

  void
  num::accept(visitor &v)
  {
    v.visit_num(*this);
  }

  std::string
  num::type_name(void) const
  {
    return "num";
  }

  std::unique_ptr<rt::mp_value>
  num::eval(env_t env)
  {
    return clone();
  }

  std::unique_ptr<rt::mp_value>
  num::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new num(*this));
  }

  bool
  num::is_truthy(void) const
  {
    // THIS TEST ALONE DOES NOT SUFFICE
    return tok.text != "0";
  }

  std::unique_ptr<rt::mp_value>
  num::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    // Support primitive messages
    // (see token_node.cxx for more)
    throw rt::dispatch_error(msg);
  }

  std::string
  num::to_str(void) const
  {
    return tok.text;
  }

  void
  swap(syntree::num &a, syntree::num &b)
  {
    using std::swap;

    swap(a.tok, b.tok);
  }
};
