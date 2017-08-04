#include "rt_utils.h"

rt_data_t *
make_nullary_expr(rt_data_t *f)
{
  rt_data_t *lst[1];
  lst[0] = f;

  rt_data_t *ret = from_list(1, lst);

  dealloc(&f);

  return ret;
}

rt_data_t *
make_unary_expr(rt_data_t *f, rt_data_t *t0)
{
  rt_data_t *lst[2];
  lst[0] = f;
  lst[1] = t0;

  rt_data_t *ret = from_list(2, lst);

  dealloc(&f);
  dealloc(&t0);

  return ret;
}

rt_data_t *
make_binary_expr(rt_data_t *f, rt_data_t *t0, rt_data_t *t1)
{
  rt_data_t *lst[3];
  lst[0] = f;
  lst[1] = t0;
  lst[2] = t1;

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

  char **frags = malloc(frag_count * sizeof(char *));
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
  free(frags);
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
      {
	/* error: => 7 chars */
	const size_t s = (data->_atom.size + 8) * sizeof (char);
	str = malloc(s);
	sprintf(str, "error: %s", data->_atom.str);
	break;
      }
    case STR:
    case ATOM:
      {
	/* Atoms already contain length information */
	const size_t s = (data->_atom.size + 1) * sizeof (char);
	str = malloc(s);
	memcpy(str, data->_atom.str, s);
	break;
      }
    case FUNC:
	str = clone_str("<function>");
	break;
    case ENV:
	str = clone_str("<environment>");
	break;
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

bool
expr_eqls(rt_data_t *lhs, rt_data_t *rhs)
{
  if (lhs == rhs) return true;
  if (!lhs || !rhs) return false; /* NULL can only equal to NULL */
  if (lhs->tag == ERR || rhs->tag == ERR) return false; /* ERR != ERR */

  if (lhs->tag == UDT)
    {
      if (lhs->_udt.equals) return lhs->_udt.equals(lhs, rhs);
      /* Default tests if objects have same mem location */
      return false;
    }

  if (rhs->tag == UDT)
    {
      if (rhs->_udt.equals) return rhs->_udt.equals(rhs, lhs);
      /* Default tests if objects have same mem location */
      return false;
    }

  /* Test for primitive types (not ERR, UDT */
  switch (lhs->tag)
    {
    case CHAR:
      switch (rhs->tag)
	{
	case CHAR:  return lhs->_char.c == rhs->_char.c;
	case INT:   return lhs->_char.c == rhs->_int.i;
	case FLOAT: return lhs->_char.c == rhs->_float.f;
	default:    return false;
	}
    case INT:
      switch (rhs->tag)
	{
	case CHAR:  return lhs->_int.i == rhs->_char.c;
	case INT:   return lhs->_int.i == rhs->_int.i;
	case FLOAT: return lhs->_int.i == rhs->_float.f;
	default:    return false;
	}
    case FLOAT:
      switch (rhs->tag)
	{
	case CHAR:  return lhs->_float.f == rhs->_char.c;
	case INT:   return lhs->_float.f == rhs->_int.i;
	case FLOAT: return lhs->_float.f == rhs->_float.f;
	default:    return false;
	}
    case FUNC:
      if (rhs->tag == FUNC) return lhs->_func.fptr == rhs->_func.fptr;
      return false;
    case ENV:
      return false;
    case ATOM:
      if (rhs->tag == ATOM)
	{
	  if (lhs->_atom.str == rhs->_atom.str) return true;
	  if (lhs->_atom.size == rhs->_atom.size)
	    {
	      return memcmp(lhs->_atom.str, rhs->_atom.str,
			    lhs->_atom.size) == 0;
	    }
	}
      return false;
    case STR:
      if (rhs->tag == STR)
	{
	  if (lhs->_atom.str == rhs->_atom.str) return true;
	  char *lhss = expr_to_str(lhs);
	  char *rhss = expr_to_str(rhs);
	  const bool ret = strcmp(lhss, rhss) == 0;
	  free(lhss);
	  free(rhss);
	  return ret;
	}
      return false;
    case ARRAY:
    case LIST:
      if (rhs->tag == ARRAY || rhs->tag == LIST)
	{
	  if (lhs->_list.list == rhs->_list.list) return true;
	  if (lhs->_list.size == rhs->_list.size)
	    {
	      /* abort as soon as a mismatch is found */
	      size_t i;
	      for (i = 0; i < lhs->_list.size; ++i)
		{
		  if (!expr_eqls(lhs->_list.list[i], rhs->_list.list[i]))
		    {
		      return false;
		    }
		}
	      return true;
	    }
	}
      return false;
    default:
      return false;
    }
}

bool
expr_cmp(rt_data_t *lhs, rt_data_t *rhs, int *ret)
{
#define RET(n)					\
  do						\
    {						\
      if (ret) *ret = n;			\
      return true;				\
    }						\
  while (0)

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

  if (lhs == rhs) RET(0);
  if (lhs == NULL || rhs == NULL) return false;
  if (lhs->tag == ERR || rhs->tag == ERR) return false;

  switch (lhs->tag)
    {
    case CHAR:
      switch (rhs->tag)
	{
	case CHAR:  RET(lhs->_char.c - rhs->_char.c);
	case INT:   RET(lhs->_char.c - rhs->_int.i);
	case FLOAT: RET(lhs->_char.c - rhs->_float.f);
	default:    return false;
	}
    case INT:
      switch (rhs->tag)
	{
	case CHAR:  RET(lhs->_int.i - rhs->_char.c);
	case INT:   RET(lhs->_int.i - rhs->_int.i);
	case FLOAT: RET(lhs->_int.i - rhs->_float.f);
	default:    return false;
	}
    case FLOAT:
      switch (rhs->tag)
	{
	case CHAR:  RET(lhs->_float.f - rhs->_char.c);
	case INT:   RET(lhs->_float.f - rhs->_int.i);
	case FLOAT: RET(lhs->_float.f - rhs->_float.f);
	default:    return false;
	}
    case ATOM:
      if (rhs->tag == ATOM) RET(strcmp(lhs->_atom.str, rhs->_atom.str));
      return false;
    case STR:
      if (rhs->tag == STR) RET(strcmp(lhs->_atom.str, rhs->_atom.str));
      return false;
    case LIST:
    case ARRAY:
      if (rhs->tag == LIST || rhs->tag == ARRAY)
	{
	  const size_t lhsl = lhs->_list.size;
	  const size_t rhsl = rhs->_list.size;
	  if (lhsl == 0) RET(-1);
	  if (rhsl == 0) RET(1);
	  const size_t max_cmp_len = lhsl > rhsl ? rhsl : lhsl;
	  size_t i;
	  int r;
	  for (i = 0; i < max_cmp_len; ++i)
	    {
	      bool flag = expr_cmp(lhs->_list.list[i], rhs->_list.list[i], &r);
	      if (flag && r != 0) RET(r);
	      if (!flag) return false;
	    }
	  /* At this point, that means both arrays are the same within the
	   * range of max_cmp_len. Now compare the array sizes.
	   */
	  RET(lhsl == rhsl ? 0 : (lhsl > rhsl ? 1 : 0));
	}
      return false;
    default:
      return false;
    }

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(pop)
#endif

#undef RET
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
impl_eqls(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  return from_int64(expr_eqls(a, b));
}

static
rt_data_t *
impl_neqls(rt_env_t *_env, rt_data_t *a, rt_data_t *b)
{
  return from_int64(!expr_eqls(a, b));
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
    case INT:   return from_double(a->_int.i / 100.0);
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
      rt_data_t *err = from_err_msg(msg);
      free(msg);
      return err;
    }
  return from_err_msg("UNKNOWN ERROR");
}

static
rt_data_t *
impl_print(rt_env_t *_env, rt_data_t *a, rt_data_t *_b)
{
  if (a)
    {
      char *msg = expr_to_str(a);
      printf(msg);
      free(msg);
      return deep_copy(a);
    }
  return from_err_msg("MISSING PARAMETERS -- print");
}


static
rt_data_t *
impl_newline(rt_env_t *_env, rt_data_t *a, rt_data_t *_b)
{
  if (a)
    {
      return from_err_msg("EXCESS PARAMETERS -- newline");
    }
  printf("\n");
  return alloc_list(0);
}

static
rt_data_t *
impl_read(rt_env_t *_env, rt_data_t *a, rt_data_t *_b)
{
  if (a)
    {
      return from_err_msg("EXCESS PARAMETERS -- read");
    }

  int ch = getchar();
  if (ch == EOF) return alloc_array(0);
  return from_char(ch);
}

static
rt_data_t *
impl_get_env (rt_env_t *env, rt_data_t *a, rt_data_t *b)
{
  if (a) return from_err_msg("EXCESS PARAMETERS -- current_env");
  return shallow_copy((rt_data_t *) env);
}

static
rt_data_t *
impl_eval (rt_env_t *env, rt_data_t *a, rt_data_t *b)
{
  if (a && b)
    {
      if (a->tag != ENV) return from_err_msg("EXPECTED ENVIRONMENT -- eval'");
      return eval(&a->_env, b);
    }

  return from_err_msg("MISSING PARAMETERS -- eval'");
}

static
rt_data_t *
impl_apply (rt_env_t *env, rt_data_t *a, rt_data_t *b)
{
  if (a && b)
    {
      if (b->tag != LIST) return from_err_msg("EXPECTED LIST -- apply'");
      return apply(env, a, &b->_list);
    }

  return from_err_msg("MISSING PARAMETERS -- apply'");
}

static
rt_data_t *
impl_compose(rt_env_t *env, rt_data_t *a, rt_data_t *b)
{
  if (a && b)
    {
      /* (-> (x) (a (b x)) */
      return eval(env,
		  make_binary_expr(from_atom("->"),
				   make_nullary_expr(from_atom("x")),
				   make_unary_expr(deep_copy(a),
						   make_unary_expr(deep_copy(b),
								   from_atom("x")))));
    }
  return from_err_msg("MISSING PARAMETERS -- .");
}

#define REL_LIKE(name, op)					\
  static							\
  rt_data_t *							\
  impl_##name (rt_env_t *_env, rt_data_t *a, rt_data_t *b)	\
  {								\
    if (a && b)							\
      {								\
	int ret;						\
	if (expr_cmp(a, b, &ret)) return from_int64(ret op 0);	\
	return from_err_msg("ILLEGAL TYPE -- " #op);		\
      }								\
    return from_err_msg("MISSING PARAMETERS -- " #op);		\
  }

REL_LIKE(less_than, <)
REL_LIKE(more_than, >)
REL_LIKE(less_eql, <=)
REL_LIKE(more_eql, >=)

#undef REL_LIKE

void
init_default_env(rt_env_t *env)
{
#define MAP_VAL(name, val)			\
  do						\
    {						\
      rt_data_t *v = val;			\
      env_define(env, name, v);			\
      dealloc(&v);				\
    }						\
  while (0)

#define DEFINE_VAL(name, val)			\
  MAP_VAL(#name, val)

  if (!env) return;

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

  DEFINE_VAL(:, from_func_ptr(impl_cons));
  DEFINE_VAL(+, from_func_ptr(impl_add));
  DEFINE_VAL(-, from_func_ptr(impl_sub));
  DEFINE_VAL(*, from_func_ptr(impl_mul));
  DEFINE_VAL(/, from_func_ptr(impl_div));
  DEFINE_VAL(mod, from_func_ptr(impl_mod));
  DEFINE_VAL(^, from_func_ptr(impl_pow));
  DEFINE_VAL(%, from_func_ptr(impl_percent));

  DEFINE_VAL(==, from_func_ptr(impl_eqls));
  DEFINE_VAL(/=, from_func_ptr(impl_neqls));

  DEFINE_VAL(<, from_func_ptr(impl_less_than));
  DEFINE_VAL(>, from_func_ptr(impl_more_than));
  DEFINE_VAL(<=, from_func_ptr(impl_less_eql));
  DEFINE_VAL(>=, from_func_ptr(impl_more_eql));

  DEFINE_VAL(., from_func_ptr(impl_compose));

  DEFINE_VAL(error, from_func_ptr(impl_error));
  DEFINE_VAL(print, from_func_ptr(impl_print));
  DEFINE_VAL(newline, from_func_ptr(impl_newline));
  DEFINE_VAL(read, from_func_ptr(impl_read));

  DEFINE_VAL(current_env, from_func_ptr(impl_get_env));
  MAP_VAL("eval'", from_func_ptr(impl_eval));
  MAP_VAL("apply'", from_func_ptr(impl_apply));

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(pop)
#endif

#undef DEFINE_VAL
#undef MAP_VAL
}
