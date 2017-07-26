#include "mp_value.hxx"

namespace rt
{
  bool
  mp_value::is_truthy(void) const
  {
    // Default all values are true
    return true;
  }
};
