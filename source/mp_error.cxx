#include "mp_error.hxx"

namespace rt
{
  static
  std::string
  gen_dispatch_error_msg(const std::string &name)
  {
    std::stringstream sstr;
    sstr << "Failed to send message: " << name;
    return sstr.str();
  }
  
  dispatch_error::dispatch_error(const std::string &name)
    : std::logic_error(gen_dispatch_error_msg(name))
  {
  }

  static
  std::string
  gen_illhand_error_msg(const std::string &msg)
  {
    std::stringstream sstr;
    sstr << "Illegal handle";
    if (msg.empty()) sstr << '!';
    else sstr << ": " << msg;
    return sstr.str();
  }

  illegal_handle_error::illegal_handle_error(const std::string &msg)
    : std::logic_error(gen_illhand_error_msg(msg))
  {
  }
};
