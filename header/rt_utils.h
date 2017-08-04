#ifndef _C_MP_RT_UTILS_H__
#define _C_MP_RT_UTILS_H__

/* There is no snprintf in VS2013 */
#if _MSC_VER && !__INTEL_COMPILER
#define _CRT_SECURE_NO_WARNINGS
#define snprintf _snprintf
#endif

#include "eval.h"
#include "utils.h"
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

  /* f, t0, t1 are invalidated after calling these functions */

  extern rt_data_t *make_nullary_expr(rt_data_t *f);

  extern rt_data_t *make_unary_expr(rt_data_t *f, rt_data_t *t0);

  extern rt_data_t *make_binary_expr(rt_data_t *f, rt_data_t *t0, rt_data_t *t1);

  /* user must dealloc data themselves */

  extern char *expr_to_str(rt_data_t *data);

  extern bool expr_is_truthy(rt_data_t *data);

  extern bool expr_eqls(rt_data_t *lhs, rt_data_t *rhs);

  /* return true if comparison is valid, false if not (for example:
   * type cannot be compared). store result in rst. Caller must
   * ensure lhs, rhs, and rst are not NULL.
   */

  extern bool expr_cmp(rt_data_t *lhs, rt_data_t *rhs, int *rst);

  /* *lhs and *rhs are swapped */

  extern void swap_expr(rt_data_t **lhs, rt_data_t **rhs);

  extern void init_default_env(rt_env_t *env);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_RT_UTILS_H__ */
