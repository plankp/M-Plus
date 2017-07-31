#include "rt_utils.h"

rt_data_t *
make_nullary_expr(rt_data_t *f)
{
  rt_data_t *lst[] = { f };
  rt_data_t *ret = from_list(1, lst);

  dealloc(&f);

  return ret;
}

rt_data_t *
make_unary_expr(rt_data_t *f, rt_data_t *t0)
{
  rt_data_t *lst[] = { f, t0 };
  rt_data_t *ret = from_list(2, lst);

  dealloc(&f);
  dealloc(&t0);

  return ret;
}

rt_data_t *
make_binary_expr(rt_data_t *f, rt_data_t *t0, rt_data_t *t1)
{
  rt_data_t *lst[] = { f, t0, t1 };
  rt_data_t *ret = from_list(3, lst);

  dealloc(&f);
  dealloc(&t0);
  dealloc(&t1);

  return ret;
}
