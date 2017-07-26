#include "cons_arr.hxx"

namespace syntree
{
  cons_arr::cons_arr(std::deque<std::shared_ptr<rt::mp_value>> _d)
    : data(_d)
  {
  }

  cons_arr::cons_arr(const syntree::cons_arr &ref)
    : data(ref.data)
  {
  }

  cons_arr::cons_arr(syntree::cons_arr &&mref)
    : cons_arr()
  {
    swap(*this, mref);
  }

  syntree::cons_arr &
  cons_arr::operator=(syntree::cons_arr obj)
  {
    swap(*this, obj);
    return *this;
  }

  void
  cons_arr::accept(visitor &v)
  {
    v.visit_cons_arr(*this);
  }

  std::string
  cons_arr::type_name(void) const
  {
    return "cons_arr";
  }

  std::unique_ptr<rt::mp_value>
  cons_arr::eval(env_t env)
  {
    return clone();
  }

  std::unique_ptr<syntree::cons_arr>
  cons_arr::cov_clone(void) const
  {
    return std::unique_ptr<syntree::cons_arr>(new syntree::cons_arr(data));
  }

  std::unique_ptr<rt::mp_value>
  cons_arr::clone(void) const
  {
    return cov_clone();
  }
  
  bool
  cons_arr::is_truthy(void) const
  {
    // An empty cons array is always false
    return data.empty();
  }

  std::unique_ptr<rt::mp_value>
  cons_arr::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
  {
    if (msg == ":")
      {
	// 1:[] => [1]
	auto ret = cov_clone();
	ret->data.push_front({ std::move(param) });
	return ret;
      }

    throw rt::dispatch_error(msg);
  }

  void swap(syntree::cons_arr &a, syntree::cons_arr &b)
  {
    using std::swap;

    swap(a.data, b.data);
  }
};
