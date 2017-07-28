#include "array.hxx"

namespace syntree
{
  array::array(std::deque<std::shared_ptr<rt::mp_value>> _d)
    : rt::mp_value(rt::type_tag::ARRAY), needs_eval(true), data(_d)
  {
  }

  array::array(const syntree::array &ref)
    : rt::mp_value(ref), needs_eval(ref.needs_eval), data(ref.data)
  {
  }

  array::array(syntree::array &&mref)
    : array()
  {
    swap(*this, mref);
  }

  syntree::array &
  array::operator=(syntree::array obj)
  {
    swap(*this, obj);
    return *this;
  }

  std::shared_ptr<rt::mp_value> &
  array::operator[](size_t idx)
  {
    return data[idx];
  }

  void
  array::push_back(std::shared_ptr<rt::mp_value> ptr)
  {
    data.push_back(ptr);
  }

  void
  array::push_front(std::shared_ptr<rt::mp_value> ptr)
  {
    data.push_front(ptr);
  }

  size_t
  array::size(void) const
  {
    return data.size();
  }
  
  void
  array::accept(visitor &v)
  {
    v.visit_array(*this);
  }

  std::string
  array::type_name(void) const
  {
    return "array";
  }

  std::unique_ptr<rt::mp_value>
  array::eval(env_t env)
  {
    if (needs_eval)
      {
	for (size_t i = 0; i < data.size(); ++i)
	  {
	    data[i] = data[i]->eval(env);
	  }
	needs_eval = false;
      }
    return clone();
  }

  std::unique_ptr<syntree::array>
  array::cov_clone(void) const
  {
    return std::unique_ptr<syntree::array>(new syntree::array(*this));
  }

  std::unique_ptr<rt::mp_value>
  array::clone(void) const
  {
    return cov_clone();
  }
  
  bool
  array::is_truthy(void) const
  {
    // An empty cons array is always false
    return !data.empty();
  }

  std::unique_ptr<rt::mp_value>
  array::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    if (needs_eval) return eval(env)->send(env, msg, std::move(param));

    if (param)
      {
	auto rhs = param->eval(env);
#define TO(type) reinterpret_cast<type*>(rhs.get())
	if (msg == ":")
	  {
	    // 1:[] => [1]
	    auto ret = cov_clone();
	    ret->data.push_front({ std::move(rhs) });
	    return ret;
	  }

	if (msg == "call")
	  {
	    if (rhs->get_type_tag() == rt::type_tag::INT)
	      {
		return data[TO(rt::mpint)->to_int()]->clone();
	      }
	    throw rt::dispatch_error(*this, msg, "rhs must be INT, found " + to_string(rhs->get_type_tag()));
	  }

	if (msg == "==" || msg == "/=") // short circuit strategy
	  {
	    if (rhs->get_type_tag() == rt::type_tag::ARRAY)
	      {
		// [0,1,2] == [1] is false seeing its different length
		auto arr = TO(syntree::array);
		if (data.size() == arr->data.size())
		  {
		    // compare each element
		    // [0,1] == [1,2] is false after seeing the first element
		    for (size_t i = 0; i < data.size(); ++i)
		      {
			auto t = data[i]->send(env, msg, arr->data[i]->clone());
			if (!t->is_truthy()) return t;
		      }
		  }
	      }
	    return std::unique_ptr<rt::mp_value>(new rt::mpint(msg == "/="));
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
  array::to_str(void) const
  {
    std::stringstream sstr;
    for (size_t i = 0; i < data.size(); ++i)
      {
	sstr << data[i]->to_str() << ':';
      }
    sstr << '[' << ']';
    return sstr.str();
  }

  void swap(syntree::array &a, syntree::array &b)
  {
    using std::swap;

    swap(static_cast<rt::mp_value&>(a), static_cast<rt::mp_value&>(b));
    swap(a.needs_eval, b.needs_eval);
    swap(a.data, b.data);
  }
};
