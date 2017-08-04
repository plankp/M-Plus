#include "rt_error.h"

rt_data_t *
create_error (rt_data_t *data)
{
  rt_error_t *err = malloc(sizeof (rt_error_t));
  err->tag = ERR;
  err->refs = 1;
  err->content = data;
  return (rt_data_t *) err;
}
