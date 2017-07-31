#ifndef _C_MP_MP_ENV_H__
#define _C_MP_MP_ENV_H__

#include "rt_data.h"

#include <string.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

  #define BUCKET_SIZE 32

  struct mp_env_pair
  {
    const char *key;
    rt_data_t *value;
    struct mp_env_pair *next;
  };
  
  typedef struct mp_env_t
  {
    rt_env_t base;		/* Inherit */
    struct mp_env_pair *bucket[BUCKET_SIZE];
  } mp_env_t;

  rt_env_t *new_mp_env(void);

  inline
  void
  print_all_buckets(void *ptr)
  {
    mp_env_t *self = (mp_env_t *) ptr;
    size_t i;
    for (i = 0; i < BUCKET_SIZE; ++i)
      {
	printf("Bucket[%zu]: %s\n", i, self->bucket[i] ? self->bucket[i]->key : "(null)");
      }
    printf("\n");
  }

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_MP_ENV_H__ */
