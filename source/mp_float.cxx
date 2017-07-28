#include "mp_float.hxx"

namespace rt
{
  mpfloat::mpfloat(double _v)
    : rt::mp_value(rt::type_tag::FLOAT), val(_v)
  {
  }

  mpfloat::mpfloat(const rt::mpfloat &ref)
    : rt::mp_value(ref), val(ref.val)
  {
  }

  mpfloat::mpfloat(rt::mpfloat &&mref)
    : mpfloat()
  {
    swap(*this, mref);
  }

  rt::mpfloat &
  mpfloat::operator=(rt::mpfloat obj)
  {
    swap(*this, obj);
    return *this;
  }

  double
  mpfloat::to_double(void) const
  {
    return val;
  }

  std::unique_ptr<rt::mp_value>
  mpfloat::eval(env_t env)
  {
    return clone();
  }

  std::unique_ptr<rt::mp_value>
  mpfloat::clone(void) const
  {
    return std::unique_ptr<rt::mp_value>(new rt::mpfloat(*this));
  }

  bool
  mpfloat::is_truthy(void) const
  {
    return val != 0;
  }

  std::unique_ptr<rt::mp_value>
  mpfloat::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
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
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val + TO(rt::mpint)->to_int()));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val + TO(rt::mpfloat)->val));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "-")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val - TO(rt::mpint)->to_int()));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val - TO(rt::mpfloat)->val));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "*" || msg == "call")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val * TO(rt::mpint)->to_int()));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val * TO(rt::mpfloat)->val));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "/")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val / TO(rt::mpint)->to_int()));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val / TO(rt::mpfloat)->val));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "mod")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(std::fmod(val, TO(rt::mpint)->to_int())));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(std::fmod(val, TO(rt::mpfloat)->val)));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "^")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(std::pow(val, TO(rt::mpint)->to_int())));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(std::pow(val, TO(rt::mpfloat)->val)));
	      default:
		throw dispatch_error(*this, msg, "rhs must be INT or FLOAT, found " + to_string(rhs->get_type_tag()));
	      }
	  }

	if (msg == "==")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val == TO(rt::mpint)->to_int()));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val == TO(rt::mpfloat)->val));
	      default:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(false));
	      }
	  }

	if (msg == "/=")
	  {
	    switch (rhs->get_type_tag())
	      {
	      case rt::type_tag::INT:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(val != TO(rt::mpint)->to_int()));
	      case rt::type_tag::FLOAT:
		return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val != TO(rt::mpfloat)->val));
	      default:
		return std::unique_ptr<rt::mp_value>(new rt::mpint(true));
	      }
	  }
#undef TO
      }
    else
      {
	if (msg == "+" || msg == "&") return clone();
	if (msg == "-") return std::unique_ptr<rt::mp_value>(new rt::mpfloat(-val));
	if (msg == "%") return std::unique_ptr<rt::mp_value>(new rt::mpfloat(val / 100.0));
      }

    throw dispatch_error(*this, msg);
  }

  std::string
  mpfloat::to_str(void) const
  {
    std::stringstream sstr;
    sstr << val;
    return sstr.str();
  }

  void
  swap(rt::mpfloat &a, rt::mpfloat &b)
  {
    using std::swap;

    swap(static_cast<rt::mp_value&>(a), static_cast<rt::mp_value&>(b));
    swap(a.val, b.val);
  }
};
