#include "i_rt_env.h"

rt_data_t *
env_look_up(rt_env_t *env, const char *id)
{
  return env->look_up(env, id);
}

void
env_mutate(rt_env_t *env, const char *id, rt_data_t *val)
{
  env->mutate(env, id, val);
}

void
env_define(rt_env_t *env, const char *id, rt_data_t *val)
{
  env->define(env, id, val);
}

void
env_remove(rt_env_t *env, const char *id)
{
  env->remove(env, id);
}

rt_data_t *
env_clone(const rt_env_t *env)
{
  return env->clone(env);
}

rt_env_t
create_base_env(void)
{
  return (rt_env_t)
    { .tag = ENV, .refs = 1 }; /* All other fields initialize as NULL */
}
