#include "mp_env.h"

static rt_data_t *mp_env_look_up (void *self, const char *id);
static void mp_env_mutate (void *self, const char *id, rt_data_t *val);
static void mp_env_define (void *self, const char *id, rt_data_t *val);
static void mp_env_remove (void *self, const char *id);
static rt_data_t *mp_env_clone (const void *self);
static void mp_env_dealloc (void *self);

static
unsigned int
djb2_hash(const char *key)
{
  unsigned int hash = 5381;
  size_t i;
  for (i = 0; key[i]; ++i)
    {
      hash = (hash << 5) + hash + (unsigned char) key[i];
    }
  return hash;
}

rt_env_t *
new_mp_env(rt_env_t *outer)
{
  mp_env_t *ret = calloc(sizeof (mp_env_t), 1);

  ret->base = create_base_env();
  ret->base.look_up = mp_env_look_up;
  ret->base.mutate = mp_env_mutate;
  ret->base.define = mp_env_define;
  ret->base.remove = mp_env_remove;
  ret->base.clone = mp_env_clone;
  ret->base.dealloc = mp_env_dealloc;

  ret->outer = outer;

  return (rt_env_t*) ret;
}

rt_data_t *
mp_env_look_up (void *_self, const char *id)
{
  mp_env_t *self = _self;
  const size_t offset = djb2_hash(id) % BUCKET_SIZE;
  struct mp_env_pair *p = self->bucket[offset];
  while (p)
    {
      if (strcmp(p->key, id) == 0) return p->value;
      p = p->next;
    }

  /* Perform lookup in outer scope */
  if (self->outer) return env_look_up(self->outer, id);
  /* Throw an error if not found */
  return from_err_msg("VARIABLE IS NOT DECLARED -- ENV-LOOKUP");
}

void
mp_env_mutate (void *_self, const char *id, rt_data_t *val)
{
  mp_env_t *self = _self;
  const size_t offset = djb2_hash(id) % BUCKET_SIZE;
  struct mp_env_pair **cell = &self->bucket[offset];
  while (*cell)
    {
      if (strcmp((*cell)->key, id) == 0)
	{
	  /* Deallocate previous value */
	  dealloc(&(*cell)->value);
	  (*cell)->value = shallow_copy(val);
	  return;
	}
      cell = &(*cell)->next;
    }

  /* Variable does not exist in this scope,
   * try mutating the parent scope (biggest
   * difference between define and mutate).
   */
  if (self->outer) env_mutate(self->outer, id, val);
}

void
mp_env_define (void *_self, const char *id, rt_data_t *val)
{
  mp_env_t *self = _self;
  const size_t offset = djb2_hash(id) % BUCKET_SIZE;
  struct mp_env_pair **cell = &self->bucket[offset];
  while (*cell)
    {
      if (strcmp((*cell)->key, id) == 0)
	{
	  /* Deallocate previous value */
	  dealloc(&(*cell)->value);
	  (*cell)->value = shallow_copy(val);
	  return;
	}
      cell = &(*cell)->next;
    }

  /* Allocate cell in place */
  *cell = calloc(sizeof (struct mp_env_pair), 1);
  (*cell)->key = clone_str(id);
  (*cell)->value = shallow_copy(val);
  (*cell)->next = NULL;
}

void
mp_env_remove (void *_self, const char *id)
{
  mp_env_t *self = _self;
  const size_t offset = djb2_hash(id) % BUCKET_SIZE;
  struct mp_env_pair **p = &self->bucket[offset];
  while (*p)
    {
      if (strcmp((*p)->key, id) == 0)
	{
	  /* Deallocate value but keep the pair */
	  dealloc(&(*p)->value);
	  (*p)->value = NULL;
	  return;
	}
      p = &(*p)->next;
    }

  /* do not remove outer scope! */
}

rt_data_t *
mp_env_clone (const void *_self)
{
  /* Deep copy current scope, shallow copy outer scope */
  const mp_env_t *self = _self;
  rt_env_t *ret = new_mp_env(&shallow_copy((rt_data_t *) self->outer)->_env);
  size_t i;
  for (i = 0; i < BUCKET_SIZE; ++i)
    {
      struct mp_env_pair *p = self->bucket[i];
      while (p)
	{
	  env_define(ret, p->key, deep_copy(p->value));
	  p = p->next;
	}
    }
  return (rt_data_t*) ret;
}

void
mp_env_dealloc (void *_self)
{
  mp_env_t *self = _self;
  size_t i;
  for (i = 0; i < BUCKET_SIZE; ++i)
    {
      struct mp_env_pair *p = self->bucket[i];
      while (p)
	{
	  dealloc(&p->value);
	  free(p->key);
	  struct mp_env_pair *next = p->next;
	  free(p);
	  p = next;
	}
    }
  rt_data_t *dr = (rt_data_t *) self->outer;
  dealloc(&dr);
}
