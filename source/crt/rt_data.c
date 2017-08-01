#include "rt_data.h"

#include <stdio.h>

static
const char *
rt_tag_to_str(const rt_tag_t tag)
{
#define X(n) case n: return #n;
  switch (tag)
    { RT_TAGS }
#undef X
  return "ILLEGAL TYPE TAG";
}

rt_data_t *
from_char(char c)
{
  // Should cache all values
  rt_data_t *ret = malloc(sizeof(rt_char_t));
  ret->tag = CHAR;
  ret->_char.c = c;
  return ret;
}

rt_data_t *
from_int64(int64_t i64)
{
  // Should cache value -128 to 128
  rt_data_t *ret = malloc(sizeof(rt_int_t));
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
from_err_msg(const char *str)
{
  rt_data_t *tmp = from_string(str);
  tmp->tag = ERR;
  return tmp;
}

rt_data_t *
from_array(size_t size, rt_data_t *ptr[])
{
  // Performs a shallow copy
  rt_data_t *tmp = alloc_array(size);
  size_t i;
  for (i = 0; i < size; ++i)
    {
      tmp->_list.list[i] = shallow_copy(ptr[i]);
    }
  return tmp;
}

rt_data_t *
from_list(size_t size, rt_data_t *ptr[])
{
  rt_data_t *ret = from_array(size, ptr);
  ret->tag = LIST;
  return ret;
}

rt_data_t *
alloc_string(size_t size)
{
  // allocates size + 1 (null-byte)
  rt_data_t *ret = malloc(sizeof (rt_atom_t) + (size + 1) * sizeof(char));
  ret->tag = STR;
  ret->_atom.refs = 1;
  ret->_atom.size = size;
  ret->_atom.str[size + 1] = '\0'; /* Set end boundary of the string */
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
  rt_data_t *ret = malloc(sizeof (rt_list_t) + size * sizeof(rt_data_t *));
  ret->tag = ARRAY;
  ret->_list.size = size;
  // ret._list.list is left untouched
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
    case ERR:
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
    case ARRAY:			/* Shallow copy all elements */
      return from_array(src->_list.size, src->_list.list);
    case LIST:			/* Shallow copy all elements */
      return from_list(src->_list.size, src->_list.list);
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
    case ERR:   return from_err_msg(src->_atom.str);
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
	case STR:
	case ATOM:
	  if (--(*src)->_atom.refs > 0) return;
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
	    size_t i;
	    for (i = 0; i < (*src)->_list.size; ++i)
	      {
		dealloc(&(*src)->_list.list[i]);
	      }
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
