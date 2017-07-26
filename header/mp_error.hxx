#ifndef _MP_MP_ERROR_HXX__
#define _MP_MP_ERROR_HXX__

#include <sstream>
#include <stdexcept>

namespace rt
{
  class dispatch_error : public std::logic_error
  {
  public:
    dispatch_error(const std::string &name = "<unknown>");
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
