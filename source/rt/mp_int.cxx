#include "mp_int.hxx"

namespace rt
{
  mpint::mpint(long long _v)
    : rt::mp_value(rt::type_tag::INT), val(_v)
  {
  }

  mpint::mpint(const rt::mpint &ref)
    : rt::mp_value(ref), val(ref.val)
  {
  }

  mpint::mpint(rt::mpint &&mref)
    : mpint()
  {
    swap(*this, mref);
  }

  rt::mpint &
  mpint::operator=(rt::mpint obj)
  {
    swap(*this, obj);
    return *this;
  }

  std::unique_ptr<rt::mp_value>
  mpint::eval(env_t)
  {
    return clone();
  }

  std::unique_ptr<rt::mp_value>
  mpint::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new rt::mpint(*this));
  }

  bool
  mpint::is_truthy(void) const
  {
    return val != 0;
  }

  std::unique_ptr<rt::mp_value>
  mpint::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    if (param)
      {
	// The following require the parameter to be evaluated
	auto rhs = param->eval(env);
#define TO(type) reinterpret_cast<type*>(rhs.get())
	if (msg == "+")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val + TO(rt::mpint)->val));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val + TO(rt::mpfloat)->to_double()));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "-")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val - TO(rt::mpint)->val));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val - TO(rt::mpfloat)->to_double()));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "*" || msg == "call")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val * TO(rt::mpint)->val));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val * TO(rt::mpfloat)->to_double()));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "/")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val / TO(rt::mpint)->val));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val / TO(rt::mpfloat)->to_double()));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "mod")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val % TO(rt::mpint)->val));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(std::fmod(val, TO(rt::mpfloat)->to_double())));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "^") // integer exponentation decays into float point
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(std::pow(val, TO(rt::mpint)->val)));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(std::pow(val, TO(rt::mpfloat)->to_double())));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "==")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val == TO(rt::mpint)->val));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val == TO(rt::mpfloat)->to_double()));
	      default:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(false));
	      }
	  }

	if (msg == "/=")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val != TO(rt::mpint)->val));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val != TO(rt::mpfloat)->to_double()));
	      default:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(true));
	      }
	  }
#undef TO
      }
    else
      {
	if (msg == "+" || msg == "&") return clone();
	if (msg == "-") return std::unique_ptr<rt::mp_value>(new rt::mpint(-val));
	if (msg == "%") return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val / 100.0));
      }

    throw dispatch_error(*this, msg);
  }

  long long
  mpint::to_int(void) const
  {
    return val;
  }

  std::string
  mpint::to_str(void) const
  {
    return std::to_string(val);
  }

  void
  swap(rt::mpint &a, rt::mpint &b)
  {
    using std::swap;

    swap(static_cast<rt::mp_value&>(a), static_cast<rt::mp_value&>(b));
    swap(a.val, b.val);
  }
};
