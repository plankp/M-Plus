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


  static
  long long
  conv_ll(const std::string& text)
  {
    if (text[0] == '0')
      {
	// Either its zero or it has a different base
	if (text.size() == 1) return 0;
	switch (text[1])
	  {
	  case 'b': return stoll(text.substr(2), nullptr, 2);
	  case 'c': return stoll(text.substr(2), nullptr, 8);
	  case 'd': return stoll(text.substr(2), nullptr, 10);
	  case 'x': return stoll(text.substr(2), nullptr, 16);
	  default:
	    throw std::invalid_argument("Unknown number literal: " + text);
	  }
      }
    return stoll(text);
  }

  std::unique_ptr<rt::mp_value>
  num::eval(env_t)
  {
    return to_rtval();
  }

  std::unique_ptr<rt::mp_value>
  num::to_rtval(void) const
  {
    if (tok.text.find('.') == std::string::npos)
      {
	return std::unique_ptr<rt::mpint>(new rt::mpint(conv_ll(tok.text)));
      }
    return std::unique_ptr<rt::mpfloat>(new rt::mpfloat(stod(tok.text)));
  }

  std::unique_ptr<rt::mp_value>
  num::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new num(*this));
  }

  bool
  num::is_truthy(void) const
  {
    return to_rtval()->is_truthy();
  }

  std::unique_ptr<rt::mp_value>
  num::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    return to_rtval()->send(env, msg, std::move(param));
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
