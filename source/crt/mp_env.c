#include "mp_env.h"

static rt_data_t *look_up (void *self, const char *id);
static void mutate (void *self, const char *id, rt_data_t *val);
static void define (void *self, const char *id, rt_data_t *val);
static void mp_env_remove (void *self, const char *id);
static rt_data_t *clone (void *self);
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
new_mp_env(void)
{
  mp_env_t *ret = calloc(sizeof (mp_env_t), 1);
  ret->base = create_base_env();
  ret->base.look_up = look_up;
  ret->base.mutate = mutate;
  ret->base.define = define;
  ret->base.remove = mp_env_remove;
  ret->base.clone = clone;
  ret->base.dealloc = mp_env_dealloc;
  return (rt_env_t*) ret;
}

rt_data_t *
look_up (void *_self, const char *id)
{
  mp_env_t *self = _self;
  const size_t offset = djb2_hash(id) % BUCKET_SIZE;
  struct mp_env_pair *p = self->bucket[offset];
  while (p)
    {
      if (strcmp(p->key, id) == 0) return p->value;
      p = p->next;
    }
  return NULL;
}

void
mutate (void *_self, const char *id, rt_data_t *val)
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
  /* Since variable does not exist, nothing happens here */
}

void
define (void *_self, const char *id, rt_data_t *val)
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
  (*cell)->key = id;
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
}

rt_data_t *
clone (void *_self)
{
  /* This is a deep clone */
  mp_env_t *self = _self;
  rt_env_t *ret = new_mp_env();
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
	  struct mp_env_pair *next = p->next;
	  free(p);
	  p = next;
	}
    }
}
