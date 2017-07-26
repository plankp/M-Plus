#include "array.hxx"

namespace syntree
{
  array::array(std::deque<std::shared_ptr<rt::mp_value>> _d)
    : data(_d)
  {
  }

  array::array(const syntree::array &ref)
    : data(ref.data)
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
    return clone();
  }

  std::unique_ptr<syntree::array>
  array::cov_clone(void) const
  {
    return std::unique_ptr<syntree::array>(new syntree::array(data));
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
    return data.empty();
  }

  std::unique_ptr<rt::mp_value>
  array::send(env_t env, const std::string &msg, std::unique_ptr<rt::mp_value> param)
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

  void swap(syntree::array &a, syntree::array &b)
  {
    using std::swap;

    swap(a.data, b.data);
  }
};
