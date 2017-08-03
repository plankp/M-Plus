#ifndef _C_MP_RT_DATA_H__
#define _C_MP_RT_DATA_H__

#include "rt_tags.h"
#include "i_rt_env.h"
#include "i_rt_udt.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct rt_char_t
  {
    rt_tag_t tag;
    char c;
  } rt_char_t;

  typedef struct rt_int_t
  {
    rt_tag_t tag;
    int64_t i;
  } rt_int_t;

  typedef struct rt_float_t
  {
    rt_tag_t tag;
    double f;
  } rt_float_t;
  
  typedef struct rt_atom_t	/* Tag must be either ATOM, STR, or ERR */
  {
    rt_tag_t tag;
    size_t refs;
    size_t size;
    bool ffree;
    char *str;
  } rt_atom_t;

  typedef struct rt_func_t
  {
    rt_tag_t tag;
    size_t refs;
    rt_data_t *(*fptr)(rt_env_t *, rt_data_t *, rt_data_t *);
  } rt_func_t;

  typedef struct rt_list_t
  {
    rt_tag_t tag;
    size_t refs;
    size_t size;
    rt_data_t **list;
  } rt_list_t;

  union rt_data_t
  {
    rt_tag_t tag;		/* Only here for accessing type tag */
    rt_char_t _char;
    rt_int_t _int;
    rt_float_t _float;
    rt_func_t _func;
    rt_env_t _env;
    rt_atom_t _atom;
    rt_list_t _list;
    rt_udt_t _udt;
  };

  extern rt_data_t *from_char(char c);

  extern rt_data_t *from_int64(int64_t lli);

  extern rt_data_t *from_double(double d);

  extern rt_data_t *from_func_ptr(rt_data_t *(*fptr)(rt_env_t *,
						     rt_data_t *,
						     rt_data_t *));

  extern rt_data_t *wrap_atom(const char *str);

  extern rt_data_t *from_atom(const char *str);

  extern rt_data_t *from_string(const char *str);

  extern rt_data_t *from_err_msg(const char *str);

  extern rt_data_t *from_list(size_t size, rt_data_t **ptr);

  extern rt_data_t *from_array(size_t size, rt_data_t **ptr);

  extern rt_data_t *alloc_string(size_t size);

  extern rt_data_t *alloc_list(size_t size);

  extern rt_data_t *alloc_array(size_t size);

  extern rt_data_t *shallow_copy(rt_data_t *src);

  extern rt_data_t *deep_copy(const rt_data_t *src);

  extern void dealloc(rt_data_t **src);

#ifdef __cplusplus
};
#endif

#endif /* !_C_MP_RT_DATA_H__ */
