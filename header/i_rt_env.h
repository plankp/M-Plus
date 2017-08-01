#ifndef _C_MP_I_RT_ENV_H__
#define _C_MP_I_RT_ENV_H__

#include "rt_tags.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct rt_env_t	/* Inherit from this interface */
  {
    rt_tag_t tag;		/* Always ENV */
    size_t refs;
    rt_data_t *(*look_up)(void *self, const char *id);
    void (*mutate)(void *self, const char *id, rt_data_t *val);
    void (*define)(void *self, const char *id, rt_data_t *val);
    void (*remove)(void *self, const char *id);
    rt_data_t *(*clone)(const void *self); /* Deep clone */
    void (*dealloc)(void *self);
  } rt_env_t;

  extern rt_data_t *env_look_up(rt_env_t *env, const char *id);

  extern void env_mutate(rt_env_t *env, const char *id, rt_data_t *val);

  extern void env_define(rt_env_t *env, const char *id, rt_data_t *val);

  extern void env_remove(rt_env_t *env, const char *id);

  extern rt_data_t *env_clone(const rt_env_t *env);

  extern rt_env_t create_base_env(void);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_I_RT_ENV_H__ */
