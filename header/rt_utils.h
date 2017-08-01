#ifndef _C_MP_RT_UTILS_H__
#define _C_MP_RT_UTILS_H__

#include "mp_env.h"
#include "rt_data.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  // f, t0, t1 are invalidated after calling these functions

  extern rt_data_t *make_nullary_expr(rt_data_t *f);

  extern rt_data_t *make_unary_expr(rt_data_t *f, rt_data_t *t0);

  extern rt_data_t *make_binary_expr(rt_data_t *f, rt_data_t *t0, rt_data_t *t1);

  // user must dealloc data themselves

  extern char *expr_to_str(rt_data_t *data);

  extern bool expr_is_truthy(rt_data_t *data);

  // *lhs and *rhs are swapped

  extern void swap_expr(rt_data_t **lhs, rt_data_t **rhs);

  extern void init_default_env(rt_env_t *env);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_RT_UTILS_H__ */
