#ifndef _C_MP_RT_UTILS_H__
#define _C_MP_RT_UTILS_H__

#include "rt_data.h"

#ifdef __cplusplus
extern "C"
{
#endif

  // f, t0, t1 are invalidated after calling these functions

  extern rt_data_t *make_nullary_expr(rt_data_t *f);

  extern rt_data_t *make_unary_expr(rt_data_t *f, rt_data_t *t0);

  extern rt_data_t *make_binary_expr(rt_data_t *f, rt_data_t *t0, rt_data_t *t1);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_RT_UTILS_H__ */
