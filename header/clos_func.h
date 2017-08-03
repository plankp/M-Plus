#ifndef _C_MP_CLOS_FUNC_H__
#define _C_MP_CLOS_FUNC_H__

#include "eval.h"
#include "utils.h"
#include "rt_data.h"
#include "i_rt_udt.h"

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct clos_func_t
  {
    rt_udt_t base;		/* Inherit */
    size_t refs;		/* Reference counter for shallow copy */
    rt_env_t *outer;		/* Reference to outer scope */
    char *param;		/* Parameter, NULL if none */
    rt_data_t *body;		/* Body of the function */
  } clos_func_t;

  /* Ownership of (env, body) are transfered from the caller */

  extern rt_udt_t *make_clos_func (rt_env_t *env, char *param, rt_data_t *body);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_CLOS_FUNC_H__ */
