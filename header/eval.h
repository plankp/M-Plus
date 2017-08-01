#ifndef _C_MP_EVAL_H__
#define _C_MP_EVAL_H__

#include "rt_data.h"
#include "rt_utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

rt_data_t *eval(rt_env_t *env, rt_data_t *data);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_EVAL_H__ */
