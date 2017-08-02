#include "i_rt_udt.h"

rt_udt_t
create_base_udt(const char *type_name)
{
  /* Initialize all other fields as NULL */
  return (rt_udt_t)
    { .tag = UDT, .type = type_name };
}
