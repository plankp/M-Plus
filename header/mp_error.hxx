#ifndef _MP_MP_ERROR_HXX__
#define _MP_MP_ERROR_HXX__

#include "mp_value.hxx"

#include <sstream>
#include <stdexcept>

namespace rt
{
  class dispatch_error : public std::logic_error
  {
  public:
    dispatch_error(const std::string &name = "<unknown>");
    dispatch_error(const rt::mp_value &recv,
		   const std::string &msg_name,
		   const std::string &info = "");
    ~dispatch_error() = default;
  };

  class illegal_handle_error : public std::logic_error
  {
  public:
    illegal_handle_error(const std::string &msg = "");
    ~illegal_handle_error() = default;
  };
};

#endif /* !_MP_MP_ERROR_HXX__ */
