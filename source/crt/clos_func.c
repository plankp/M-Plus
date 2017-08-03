#include "clos_func.h"

static rt_data_t *s_sclone (void *self);
static rt_data_t *s_dclone (const void *self);
static char *s_to_str (const void *self);
static bool s_to_bool (const void *self);
static bool s_equals (const void *self, const rt_data_t *obj);
static rt_data_t *s_apply (void *self, rt_env_t *e, rt_data_t *p);
static bool s_dealloc (void *self);

rt_udt_t *
make_clos_func (rt_env_t *env, char *param, rt_data_t *body)
{
  clos_func_t *ret = calloc(1, sizeof (clos_func_t));

  ret->base = create_base_udt("<function>");
  ret->base.sclone = s_sclone;
  ret->base.dclone = s_dclone;
  ret->base.to_str = s_to_str;
  ret->base.to_bool = s_to_bool;
  ret->base.equals = s_equals;
  ret->base.apply = s_apply;
  ret->base.dealloc = s_dealloc;

  ret->refs = 1;
  ret->outer = env;
  ret->param = clone_str(param);
  ret->body = body;
  return (rt_udt_t *) ret;
}

rt_data_t *
s_sclone (void *_self)
{
  clos_func_t *self = _self;
  ++self->refs;
  return _self;
}

rt_data_t *
s_dclone (const void *_self)
{
  const clos_func_t *self = _self;
  /* Always shallow clone the environment */
  return (rt_data_t *)
    make_clos_func(&shallow_copy((rt_data_t *) self->outer)->_env,
		   self->param, deep_copy(self->body));
}

char *
s_to_str (const void *_self)
{
  const clos_func_t *self = _self;
  return clone_str(self->base.type);
}

bool
s_to_bool (const void *self)
{
  return true;
}

bool
s_equals (const void *self, const rt_data_t *obj)
{
  return false;
}

rt_data_t *
s_apply (void *_self, rt_env_t *_e, rt_data_t *p)
{
  /* Ignore the environment it is evaluated in (_e) */
  clos_func_t *self = _self;
  if (self->param)
    {
      if (p)
	{
	  env_define(self->outer, self->param, p);
	  return eval(self->outer, self->body);
	}
      return from_err_msg("MISSING PARAMETER -- APPLY");
    }
  if (p) return from_err_msg("EXCESS PARAMETER -- APPLY");
  return eval(self->outer, self->body);
}

bool
s_dealloc (void *_self)
{
  clos_func_t *self = _self;
  if (--self->refs > 0) return false;
  rt_data_t *dr = (rt_data_t *) self->outer;
  dealloc(&dr);
  free(self->param);
  dealloc(&self->body);
  return true;
}
