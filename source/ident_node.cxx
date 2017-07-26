#include "ident_node.hxx"

namespace syntree
{
  ident::ident(mp_token_t _tok)
    : tok(_tok)
  {
  }

  ident::ident(const syntree::ident &ref)
    : ident(ref.tok)
  {
  }

  ident::ident(syntree::ident &&ref)
    : ident()
  {
    swap(*this, ref);
  }

  ident::~ident()
  {
  }

  syntree::ident &
  ident::operator=(syntree::ident obj)
  {
    swap(*this, obj);
    return *this;
  }

  mp_token_t
  ident::get_token(void)
  {
    return tok;
  }

  void
  ident::accept(visitor &v)
  {
    v.visit_ident(*this);
  }

  std::string
  ident::type_name(void) const
  {
    return "ident";
  }

  std::unique_ptr<rt::mp_value>
  ident::eval(env_t env)
  {
    try
      { return env.at(tok.text)->clone(); }
    catch (std::out_of_range &e)
      { throw rt::illegal_handle_error(tok.text); }
  }

  std::unique_ptr<rt::mp_value>
  ident::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new ident(*this));
  }

  std::unique_ptr<rt::mp_value>
  ident::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    // Primitive messages:
    //   - <-
    //   - =
    //   - ->

    if (msg == "<-")
      {
	auto rhs = param->eval(env);
	try
	  { env.at(tok.text) = { rhs->clone() }; }
	catch (std::out_of_range &e)
	  { throw rt::illegal_handle_error(tok.text); }
	return rhs;
      }

    if (msg == "=")
      {
	auto rhs = param->eval(env);
	env[tok.text] = { rhs->clone() };
	return rhs;
      }

    if (msg == "->")
      {
	// Do not process parameter
	return std::unique_ptr<rt::udef_func>(new rt::udef_func(tok.text, std::move(param), env));
      }

    // Runtime wanted to send to the value referenced by the identifier
    return eval(env)->send(env, msg, std::move(param));
  }

  std::string
  ident::to_str(void) const
  {
    return tok.text;
  }

  void
  swap(syntree::ident &a, syntree::ident &b)
  {
    using std::swap;

    swap(a.tok, b.tok);
  }
};
