#ifndef _C_MP_EVAL_H__
#define _C_MP_EVAL_H__

#include "rt_data.h"
#include "rt_utils.h"
#include "clos_func.h"

#ifdef __cplusplus
extern "C"
{
#endif

  extern rt_data_t *eval(rt_env_t *env, rt_data_t *data);

  extern rt_data_t *apply(rt_env_t *env, rt_data_t *val, rt_list_t *param);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_EVAL_H__ */
