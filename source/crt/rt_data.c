#include "rt_data.h"

#include <stdio.h>

rt_data_t *
from_char(char c)
{
  /* Should cache all values */
  rt_data_t *ret = malloc(sizeof(rt_char_t));
  ret->tag = CHAR;
  ret->_char.c = c;
  return ret;
}

rt_data_t *
from_int64(int64_t i64)
{
  /* Should cache value -128 to 128 */
  rt_data_t *ret = calloc(1, sizeof(rt_int_t));
  ret->tag = INT;
  ret->_int.i = i64;
  return ret;
}

rt_data_t *
from_double(double d)
{
  rt_data_t *ret = malloc(sizeof(rt_data_t));
  ret->tag = FLOAT;
  ret->_float.f = d;
  return ret;
}

rt_data_t *
from_func_ptr(rt_data_t *(*fptr)(rt_env_t *, rt_data_t *, rt_data_t *))
{
  rt_data_t *ret = malloc(sizeof(rt_data_t));
  ret->tag = FUNC;
  ret->_func.refs = 1;
  ret->_func.fptr = fptr;
  return ret;
}

rt_data_t *
wrap_atom(const char *str)
{
  rt_data_t *tmp = malloc(sizeof (rt_atom_t));
  tmp->tag = ATOM;
  tmp->_atom.refs = 1;
  tmp->_atom.size = strlen(str);
  tmp->_atom.ffree = false;	/* Wrapping assumes the caller will cleanup */
  tmp->_atom.str = str;
  return tmp;
}

rt_data_t *
from_atom(const char *str)
{
  rt_data_t *tmp = from_string(str);
  tmp->tag = ATOM;
  return tmp;
}

rt_data_t *
from_string(const char *str)
{
  size_t len = strlen(str);
  rt_data_t *tmp = alloc_string(len);
  if (str) memcpy(tmp->_atom.str, str, (len + 1) * sizeof(char));
  return tmp;
}

rt_data_t *
from_array(size_t size, rt_data_t **ptr)
{
  /* Performs a shallow copy */
  rt_data_t *tmp = alloc_array(size);
  size_t i;
  for (i = 0; i < size; ++i)
    {
      tmp->_list.list[i] = shallow_copy(ptr[i]);
    }
  return tmp;
}

rt_data_t *
from_list(size_t size, rt_data_t **ptr)
{
  rt_data_t *ret = from_array(size, ptr);
  ret->tag = LIST;
  return ret;
}

rt_data_t *
alloc_string(size_t size)
{
  /* allocates size + 1 (null-byte) */
  rt_data_t *ret = calloc(1, sizeof (rt_atom_t));
  ret->tag = STR;
  ret->_atom.refs = 1;
  ret->_atom.size = size;
  ret->_atom.ffree = true;	   /* This atom is dynamically allocated */
  ret->_atom.str = malloc(size * sizeof(char) + 1);
  return ret;
}

rt_data_t *
alloc_list(size_t size)
{
  rt_data_t *ret = alloc_array(size);
  ret->tag = LIST;
  return ret;
}

rt_data_t *
alloc_array(size_t size)
{
  rt_data_t *ret = calloc(1, sizeof (rt_list_t));
  ret->tag = ARRAY;
  ret->_list.refs = 1;
  ret->_list.size = size;
  ret->_list.list = size == 0 ? NULL : calloc(sizeof(rt_data_t *), size);
  return ret;
}

rt_data_t *
shallow_copy(rt_data_t *src)
{
  if (src == NULL) return NULL;
  switch (src->tag)
    {
      /* Primitives always deep copy */
    case CHAR:  return from_char(src->_char.c);
    case INT:   return from_int64(src->_int.i);
    case FLOAT: return from_double(src->_float.f);
    case ERR:			/* Increase reference counter */
      ++src->_err.refs;
      return src;
    case STR:
    case ATOM:			/* Increase reference counter */
      ++src->_atom.refs;
      return src;
    case FUNC:			/* Increase reference counter */
      ++src->_func.refs;
      return src;
    case ENV:			/* Increase reference counter */
      ++src->_env.refs;
      return src;
    case UDT:			/* Based on custom implementation */
      return src->_udt.sclone(src);
    case ARRAY:
    case LIST:			/* Increase reference counter */
      ++src->_list.refs;
      return src;
    default:
      fprintf(stderr, "Attempt to shallow copy type: %s(%d)\n", rt_tag_to_str(src->tag), src->tag);
      assert(false);
    }
}

rt_data_t *
deep_copy(const rt_data_t *src)
{
  if (src == NULL) return NULL;
  switch (src->tag)
    {
      /* Primitives always deep copy */
    case CHAR:  return from_char(src->_char.c);
    case INT:   return from_int64(src->_int.i);
    case FLOAT: return from_double(src->_float.f);
    case ATOM:  return from_atom(src->_atom.str);
    case STR:   return from_string(src->_atom.str);
    case ERR:   return create_error(shallow_copy(src->_err.content));
    case FUNC:  return from_func_ptr(src->_func.fptr);
    case ENV:   return env_clone(&src->_env);
    case UDT:   return src->_udt.dclone(src);
    case ARRAY:
    case LIST:			/* Copy every element */
      {
	rt_data_t *tmp = alloc_array(src->_list.size);
	size_t i;
	for (i = 0; i < src->_list.size; ++i)
	  {
	    tmp->_list.list[i] = deep_copy(src->_list.list[i]);
	  }
	tmp->tag = src->tag;
	return tmp;
      }
    default:
      assert(false);
    }
}

void
dealloc(rt_data_t **src)
{
  if (src && *src)
    {
      switch ((*src)->tag)
	{
	case CHAR:
	case INT:
	case FLOAT:		/* Nothing is dynamically allocated */
	  break;
	case FUNC:
	  if (--(*src)->_func.refs > 0) return;
	  break;
	case ERR:
	  if (--(*src)->_err.refs > 0) return;
	  dealloc(&(*src)->_err.content);
	  break;
	case STR:
	case ATOM:
	  if (--(*src)->_atom.refs > 0) return;
	  if ((*src)->_atom.ffree) free((*src)->_atom.str);
	  break;
	case ENV:
	  if (--(*src)->_env.refs > 0) return;
	  (*src)->_env.dealloc(*src);
	  break;
	case UDT:
	  if (!(*src)->_udt.dealloc(*src)) return;
	  break;
	case ARRAY:
	case LIST:		/* Deallocate every element */
	  {
	    if (--(*src)->_list.refs > 0) return;
	    size_t i;
	    for (i = 0; i < (*src)->_list.size; ++i)
	      {
		dealloc(&(*src)->_list.list[i]);
	      }
	    free((*src)->_list.list);
	    break;
	  }
	default:
	  fprintf(stderr, "Attempt to free type: %s(%d)\n",
		  rt_tag_to_str((*src)->tag),
		  (*src)->tag);
	  assert(false);
	}
      free(*src);
    }
}

rt_data_t *
from_err_msg(const char *str)
{
  return create_error(from_string(str));
}
