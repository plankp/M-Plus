#include "mp_error.hxx"

namespace rt
{
  static
  std::string
  gen_dispatch_error_msg(const rt::mp_value *recv,
			 const std::string &name,
			 const std::string &info)
  {
    std::stringstream sstr;
    sstr << "Failed to send message";
    if (recv != nullptr) sstr << " to " << to_string(recv->get_type_tag());
    sstr << ": " << name;
    if (!info.empty())
      {
	sstr << " [" << info << ']';
      }
    return sstr.str();
  }
  
  dispatch_error::dispatch_error(const std::string &name)
    : std::logic_error(gen_dispatch_error_msg(nullptr, name, ""))
  {
  }

  dispatch_error::dispatch_error(const rt::mp_value &recv,
				 const std::string &msg_name,
				 const std::string &info)
    : std::logic_error(gen_dispatch_error_msg(&recv, msg_name, info))
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
