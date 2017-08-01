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

static
char *
generate(rt_data_t *data,
	 const char start, const char end, const char sep)
{
  char *str = NULL;
  const size_t frag_count = data->_list.size;
  if (frag_count == 0)
    {
      str = malloc(3);
      str[0] = start;
      str[1] = end;
      str[2] = '\0';
      return str;
    }

  char *frags[frag_count];
  size_t i;
  for (i = 0; i < frag_count; ++i)
    {
      frags[i] = expr_to_str(data->_list.list[i]);
    }

  size_t s = 2;
  for (i = 0; i < frag_count; ++i)
    {
      s += snprintf(NULL, 0, "%s", frags[i]) + 1;
    }

  str = malloc(s * sizeof(char));
  for (s = 0, i = 0; i < frag_count; ++i)
    {
      s += sprintf(str + s, "%c%s", sep, frags[i]);
      free(frags[i]);
    }
  str[s] = end;
  str[s + 1] = '\0';
  str[0] = start;
  return str;
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
    case ATOM:
      {
	// Atoms already contain length information
	const size_t s = (data->_atom.size + 1) * sizeof (char);
	str = malloc(s);
	memcpy(str, data->_atom.str, s);
	break;
      }
    case STR:
      {
	// Preserve '@', remove quoting
	size_t s = data->_atom.size;
	if (s > 2 && data->_atom.str[1] == '"')	/* @"" is at least 3 chars */
	  {
	    s -= 2;
	  }
	str = malloc((s + 1) * sizeof (char));
	if (s == data->_atom.size) memcpy(str, data->_atom.str, s + 1);
	else
	  {
	    memcpy(str + 1, data->_atom.str + 2, s);
	    str[0] = '@';
	    str[s] = '\0';
	  }
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
    case UDT:
      if (data->_udt.to_str) str = data->_udt.to_str(data);
      else
	{
	  const size_t s = strlen(data->_udt.type) + 3;
	  str = malloc(s * sizeof (char));
	  sprintf(str, "<%s>", data->_udt.type);
	}
      break;
    case LIST:
      str = generate(data, '(', ')', ' ');
      break;
    case ARRAY:
      str = generate(data, '[', ']', ',');
      break;
    default:
      return NULL;
    }
  return str;
}

bool
expr_is_truthy (rt_data_t *data)
{
  if (!data) return false;
  switch (data->tag)
    {
    case CHAR:  return data->_char.c != 0;
    case INT:   return data->_int.i != 0;
    case FLOAT: return data->_float.f != 0;
    case ATOM:  return data->_atom.size != 0;
    case STR:   return !(data->_atom.size == 0
			 || strcmp("@", data->_atom.str) == 0
			 || strcmp("@\"\"", data->_atom.str) == 0);
    case ERR:   return false;
    case FUNC:  return true;
    case ENV:   return true;
    case UDT:   return data->_udt.to_bool(data);
    case ARRAY:
    case LIST:  return data->_list.size != 0;
    default:    return true;
    }
}

void
swap_expr(rt_data_t **lhs, rt_data_t **rhs)
{
  rt_data_t *tmp = *lhs;
  *lhs = *rhs;
  *rhs = tmp;
}

static
rt_data_t *
impl_cons(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  if (a && b)
    {
      switch (b->tag)
	{
	case ARRAY:
	case LIST:
	  {
	    /* This relies on ARRAY and LIST having the same structure */

	    rt_data_t *ret = alloc_list(b->_list.size + 1);
	    ret->_list.list[0] = shallow_copy(a);
	    size_t i;
	    for (i = 0; i < b->_list.size; ++i)
	      {
		ret->_list.list[i + 1] = shallow_copy(b->_list.list[i]);
	      }
	    ret->tag = b->tag;
	    return ret;
	  }
	default:
	  break;
	}
      return from_err_msg("ILLEGAL TYPE -- :");
    }
  return from_err_msg("MISSING PARAMETERS -- :");
}

static
rt_data_t *
impl_add(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  if (b)
    {
      /* Binary add */
      switch (a->tag)
	{
	case CHAR:
	  switch (b->tag)
	    {
	    case CHAR:  return from_char(a->_char.c + b->_char.c);
	    case INT:   return from_int64(a->_char.c + b->_int.i);
	    case FLOAT: return from_double(a->_char.c + b->_float.f);
	    default:    break;
	    }
	  break;
	case INT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_int64(a->_int.i + b->_char.c);
	    case INT:   return from_int64(a->_int.i + b->_int.i);
	    case FLOAT: return from_double(a->_int.i + b->_float.f);
	    default:    break;
	    }
	  break;
	case FLOAT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(a->_float.f + b->_char.c);
	    case INT:   return from_double(a->_float.f + b->_int.i);
	    case FLOAT: return from_double(a->_float.f + b->_float.f);
	    default:    break;
	    }
	  break;
	default:
	  break;
	}
      return from_err_msg("ILLEGAL TYPE -- +");
    }

  /* Unary add */
  if (a->tag == INT || a->tag == FLOAT || a->tag == CHAR)
    {
      return shallow_copy(a);
    }
  return from_err_msg("ILLEGAL TYPE -- UNARY +");
}

static
rt_data_t *
impl_sub(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  if (b)
    {
      /* Binary minus */
      switch (a->tag)
	{
	case CHAR:
	  switch (b->tag)
	    {
	    case CHAR:  return from_char(a->_char.c - b->_char.c);
	    case INT:   return from_int64(a->_char.c - b->_int.i);
	    case FLOAT: return from_double(a->_char.c - b->_float.f);
	    default:    break;
	    }
	  break;
	case INT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_int64(a->_int.i - b->_char.c);
	    case INT:   return from_int64(a->_int.i - b->_int.i);
	    case FLOAT: return from_double(a->_int.i - b->_float.f);
	    default:    break;
	    }
	  break;
	case FLOAT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(a->_float.f - b->_char.c);
	    case INT:   return from_double(a->_float.f - b->_int.i);
	    case FLOAT: return from_double(a->_float.f - b->_float.f);
	    default:    break;
	    }
	  break;
	default:
	  break;
	}
      return from_err_msg("ILLEGAL TYPE -- -");
    }

  /* Unary minus */
  switch (a->tag)
    {
    case CHAR:  return from_char(-a->_char.c);
    case INT:   return from_int64(-a->_int.i);
    case FLOAT: return from_double(-a->_float.f);
    default:    return from_err_msg("ILLEGAL TYPE -- UNARY -");
    }
}

static
rt_data_t *
impl_mul(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  if (b)
    {
      /* Binary mul */
      switch (a->tag)
	{
	case CHAR:
	  switch (b->tag)
	    {
	    case CHAR:  return from_char(a->_char.c * b->_char.c);
	    case INT:   return from_int64(a->_char.c * b->_int.i);
	    case FLOAT: return from_double(a->_char.c * b->_float.f);
	    default:    break;
	    }
	  break;
	case INT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_int64(a->_int.i * b->_char.c);
	    case INT:   return from_int64(a->_int.i * b->_int.i);
	    case FLOAT: return from_double(a->_int.i * b->_float.f);
	    default:    break;
	    }
	  break;
	case FLOAT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(a->_float.f * b->_char.c);
	    case INT:   return from_double(a->_float.f * b->_int.i);
	    case FLOAT: return from_double(a->_float.f * b->_float.f);
	    default:    break;
	    }
	  break;
	default:
	  break;
	}
      return from_err_msg("ILLEGAL TYPE -- *");
    }

  return from_err_msg("MISSING PARAMETERS -- *");
}

static
rt_data_t *
impl_div(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  if (b)
    {
      /* Binary div */
      switch (a->tag)
	{
	case CHAR:
	  switch (b->tag)
	    {
	    case CHAR:  return from_char(a->_char.c / b->_char.c);
	    case INT:   return from_int64(a->_char.c / b->_int.i);
	    case FLOAT: return from_double(a->_char.c / b->_float.f);
	    default:    break;
	    }
	  break;
	case INT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_int64(a->_int.i / b->_char.c);
	    case INT:   return from_int64(a->_int.i / b->_int.i);
	    case FLOAT: return from_double(a->_int.i / b->_float.f);
	    default:    break;
	    }
	  break;
	case FLOAT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(a->_float.f / b->_char.c);
	    case INT:   return from_double(a->_float.f / b->_int.i);
	    case FLOAT: return from_double(a->_float.f / b->_float.f);
	    default:    break;
	    }
	  break;
	default:
	  break;
	}
      return from_err_msg("ILLEGAL TYPE -- /");
    }

  return from_err_msg("MISSING PARAMETERS -- /");
}

static
rt_data_t *
impl_mod(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  if (b)
    {
      /* Binary mod */
      switch (a->tag)
	{
	case CHAR:
	  switch (b->tag)
	    {
	    case CHAR:  return from_char(a->_char.c % b->_char.c);
	    case INT:   return from_int64(a->_char.c % b->_int.i);
	    case FLOAT: return from_double(fmod(a->_char.c, b->_float.f));
	    default:    break;
	    }
	  break;
	case INT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_int64(a->_int.i % b->_char.c);
	    case INT:   return from_int64(a->_int.i % b->_int.i);
	    case FLOAT: return from_double(fmod(a->_int.i, b->_float.f));
	    default:    break;
	    }
	  break;
	case FLOAT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(fmod(a->_float.f, b->_char.c));
	    case INT:   return from_double(fmod(a->_float.f, b->_int.i));
	    case FLOAT: return from_double(fmod(a->_float.f, b->_float.f));
	    default:    break;
	    }
	  break;
	default:
	  break;
	}
      return from_err_msg("ILLEGAL TYPE -- mod");
    }

  return from_err_msg("MISSING PARAMETERS -- mod");
}

static
rt_data_t *
impl_pow(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  if (b)
    {
      /* Binary pow */
      switch (a->tag)
	{
	case CHAR:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(pow(a->_char.c, b->_char.c));
	    case INT:   return from_double(pow(a->_char.c, b->_int.i));
	    case FLOAT: return from_double(pow(a->_char.c, b->_float.f));
	    default:    break;
	    }
	  break;
	case INT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(pow(a->_int.i, b->_char.c));
	    case INT:   return from_double(pow(a->_int.i, b->_int.i));
	    case FLOAT: return from_double(pow(a->_int.i, b->_float.f));
	    default:    break;
	    }
	  break;
	case FLOAT:
	  switch (b->tag)
	    {
	    case CHAR:  return from_double(pow(a->_float.f, b->_char.c));
	    case INT:   return from_double(pow(a->_float.f, b->_int.i));
	    case FLOAT: return from_double(pow(a->_float.f, b->_float.f));
	    default:    break;
	    }
	  break;
	default:
	  break;
	}
      return from_err_msg("ILLEGAL TYPE -- ^");
    }

  return from_err_msg("MISSING PARAMETERS -- ^");
}

static
rt_data_t *
impl_percent(rt_env_t *_env, rt_data_t *a, rt_data_t *_b)
{
  if (_b) return from_err_msg("EXCESS PARAMETERS -- %");

  switch (a->tag)
    {
    case INT:   return from_double(a->_int.i / 100);
    case FLOAT: return from_double(a->_float.f / 100.0);
    default:    return from_err_msg("ILLEGAL TYPE -- %");
    }
}

static
rt_data_t *
impl_error(rt_env_t *_env, rt_data_t *a, rt_data_t *_b)
{
  if (a)
    {
      char *msg = expr_to_str(a);
      from_err_msg(msg);
      free(msg);
    }
  return from_err_msg("UNKNOWN ERROR");
}

void
init_default_env(rt_env_t *env)
{
#define DEFINE_VAL(name, val)			\
  do						\
    {						\
      rt_data_t *v = val;			\
      env_define(env, #name, v);		\
      dealloc(&v);				\
    }						\
  while (0)

  if (!env) return;
  DEFINE_VAL(error, from_func_ptr(impl_error));
  DEFINE_VAL(:, from_func_ptr(impl_cons));
  DEFINE_VAL(+, from_func_ptr(impl_add));
  DEFINE_VAL(-, from_func_ptr(impl_sub));
  DEFINE_VAL(*, from_func_ptr(impl_mul));
  DEFINE_VAL(/, from_func_ptr(impl_div));
  DEFINE_VAL(mod, from_func_ptr(impl_mod));
  DEFINE_VAL(^, from_func_ptr(impl_pow));
  DEFINE_VAL(%, from_func_ptr(impl_percent));

#undef DEFINE_VAL
}
