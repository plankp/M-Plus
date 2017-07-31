#include "rt_utils.h"

rt_data_t *
make_nullary_expr(rt_data_t *f)
{
  rt_data_t *lst[] = { f };
  rt_data_t *ret = from_list(1, lst);

  dealloc(&f);

  return ret;
}

rt_data_t *
make_unary_expr(rt_data_t *f, rt_data_t *t0)
{
  rt_data_t *lst[] = { f, t0 };
  rt_data_t *ret = from_list(2, lst);

  dealloc(&f);
  dealloc(&t0);

  return ret;
}

rt_data_t *
make_binary_expr(rt_data_t *f, rt_data_t *t0, rt_data_t *t1)
{
  rt_data_t *lst[] = { f, t0, t1 };
  rt_data_t *ret = from_list(3, lst);

  dealloc(&f);
  dealloc(&t0);
  dealloc(&t1);

  return ret;
}

char *
expr_to_str(rt_data_t *data)
{
  if (!data) return NULL;
  char *str = NULL;
  switch (data->tag)
    {
    case CHAR:
      str = malloc(2 * sizeof (char));
      str[0] = data->_char.c;
      str[1] = '\0';
      break;
    case INT:
      {
	const size_t s = snprintf(NULL, 0, "%" PRId64, data->_int.i) + 1;
	str = malloc(s * sizeof (char));
	snprintf(str, s, "%" PRId64, data->_int.i);
	break;
      }
    case FLOAT:
      {
	const size_t s = snprintf(NULL, 0, "%g", data->_float.f) + 1;
	str = malloc(s * sizeof (char));
	snprintf(str, s, "%g", data->_float.f);
	break;
      }
    case ERR:
    case STR:
    case ATOM:
      {
	// Atoms already contain length information
	const size_t s = (data->_atom.size + 1) * sizeof (char);
	str = malloc(s);
	memcpy(str, data->_atom.str, s);
	break;
      }
    case FUNC:
      {
	const char *tmp = "<function>";
	const size_t s = (strlen(tmp) + 1) * sizeof(char);
	str = malloc(s);
	memcpy(str, tmp, s);
	break;
      }
    case ENV:
      {
	const char *tmp = "<environment>";
	const size_t s = (strlen(tmp) + 1) * sizeof(char);
	str = malloc(s);
	memcpy(str, tmp, s);
	break;
      }
    case LIST:
      {
	const size_t frag_count = data->_list.size;
	if (frag_count == 0)
	  {
	    str = malloc(3);
	    str[0] = '(';
	    str[1] = ')';
	    str[2] = '\0';
	    break;
	  }

	char *frags[frag_count];
	size_t i;
	for (i = 0; i < frag_count; ++i)
	  {
	    frags[i] = expr_to_str(data->_list.list[i]);
	  }

	// '(', ')'
        size_t s = 2;
	for (i = 0; i < frag_count; ++i)
	  {
	    s += snprintf(NULL, 0, "%s", frags[i]) + 1;
	  }

	str = malloc(s * sizeof(char));
	for (s = 0, i = 0; i < frag_count; ++i)
	  {
	    s += sprintf(str + s, " %s", frags[i]);
	    free(frags[i]);
	  }
	str[s] = ')';
	str[s + 1] = '\0';
	str[0] = '(';
	break;
      }
    default:
      return NULL;
    }
  return str;
}
