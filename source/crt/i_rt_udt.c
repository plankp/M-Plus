#include "i_rt_udt.h"

rt_udt_t
create_base_udt(const char *type_name)
{
  /* Initialize all other fields as NULL */
  rt_udt_t ret = (rt_udt_t) { .tag = UDT };
  ret.type = type_name;
 return ret;
}
