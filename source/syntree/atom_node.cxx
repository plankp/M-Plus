#include "atom_node.hxx"

namespace syntree
{
  atom::atom(mp_token_t t)
    : rt::mp_value(rt::type_tag::ATOM), tok(t)
  {
    if (t.type != mp_token_t::L_ATOM)
      {
	throw std::invalid_argument("Expected L_ATOM token");
      }
  }

  atom::atom(const syntree::atom &ref)
    : rt::mp_value(ref), tok(ref.tok)
  {
  }

  atom::atom(syntree::atom &&mref)
    : atom()
  {
    swap(*this, mref);
  }

  syntree::atom &
  atom::operator=(syntree::atom obj)
  {
    swap(*this, obj);
    return *this;
  }

  mp_token_t
  atom::get_token(void)
  {
    return tok;
  }

  void
  atom::accept(visitor &v)
  {
    v.visit_atom(*this);
  }

  std::string
  atom::type_name(void) const
  {
    return "atom";
  }

  std::unique_ptr<rt::mp_value>
  atom::eval(env_t)
  {
    return clone();
  }

  std::unique_ptr<rt::mp_value>
  atom::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new atom(*this));
  }

  bool
  atom::is_truthy(void) const
  {
    return !(tok.text == "@" || tok.text == "@\"\"");
  }

  std::unique_ptr<rt::mp_value>
  atom::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    if (param)
      {
	auto rhs = param->eval(env);
#define TO(type) reinterpret_cast<type*>(rhs.get())
	if (msg == "call")
	  {
	    if (rhs->get_type_tag() == rt::type_tag::INT)
	      {
		return std::unique_ptr<rt::mp_value>(new rt::mpint(to_str()[TO(rt::mpint)->to_int()]));
	      }
	  }

	if (msg == "==")
	  {
	    if (rhs->get_type_tag() == rt::type_tag::ATOM)
	      {
		return std::unique_ptr<rt::mp_value>(new rt::mpint(to_str() == TO(syntree::atom)->to_str()));
	      }
	    return std::unique_ptr<rt::mp_value>(new rt::mpint(false));
	  }

	if (msg == "/=")
	  {
	    if (rhs->get_type_tag() == rt::type_tag::ATOM)
	      {
		return std::unique_ptr<rt::mp_value>(new rt::mpint(to_str() != TO(syntree::atom)->to_str()));
	      }
	    return std::unique_ptr<rt::mp_value>(new rt::mpint(true));
	  }
#undef TO
      }
    else
      {
	if (msg == "&") return clone();
      }
    throw rt::dispatch_error(*this, msg);
  }

  std::string
  atom::to_str(void) const
  {
    if (is_truthy())
      {
	if (tok.text[1] == '\"')
	  {
	    // @"ABC" returns ABC
	    return tok.text.substr(2, tok.text.size() - 3);
	  }
	// @ABC returns ABC
	return tok.text.substr(1);
      }
    // A false atom is always empty
    return "";
  }

  void
  swap(syntree::atom &a, syntree::atom &b)
  {
    using std::swap;

    swap(static_cast<rt::mp_value&>(a), static_cast<rt::mp_value&>(b));
    swap(a.tok, b.tok);
  }
};
