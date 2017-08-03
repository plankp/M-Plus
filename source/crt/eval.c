#include "eval.h"

#define GUARD_DO(name, value, id)				\
  rt_data_t *name = value;					\
  if (!name)							\
    {								\
      ret = from_err_msg("TYPE IS NULL -- EVAL");		\
      goto name## _ ##id;					\
    }								\
  if (name->tag == ERR)						\
    {								\
      ret = name;						\
      goto name## _ ##id;					\
    }								\
      {

#define GUARD_END(name, id)			\
      dealloc(&name);				\
    }						\
      name## _ ##id:

static rt_data_t *intern_eval(rt_env_t *env, rt_data_t *data,
			      const bool atm_should_dcpy);

static rt_data_t *proc_assignment(rt_env_t *env, rt_data_t *data,
				  const bool should_def_new);

static
rt_data_t *
proc_assignment(rt_env_t *env, rt_data_t *data, const bool should_def_new)
{
  rt_data_t *ret = NULL;
  GUARD_DO(rhs, eval(env, data->_list.list[2]), def);
  if (data->_list.list[1]->tag == ATOM)
    {
      if (should_def_new)
	{ env_define(env, data->_list.list[1]->_atom.str, rhs); }
      else
	{ env_mutate(env, data->_list.list[1]->_atom.str, rhs); }
    }
  else
    {
      /* Swap the lhs and rhs, then deallocate rhs after
       * swapping. Note: deallocate is guaranteed by the
       * GUARD_END clause!
       */
      rt_data_t *lhs = intern_eval(env, data, false);
      swap_expr(&lhs, &rhs);
    }
  GUARD_END(rhs, def);
  return ret;
}

rt_data_t *
intern_eval(rt_env_t *env, rt_data_t *data, const bool atm_should_cpy)
{
  if (!data) return NULL;
  switch (data->tag)
    {
    case CHAR:
    case INT:
    case FLOAT:
    case FUNC:
    case ERR:
    case STR:
    case ARRAY:
    case UDT:
    case ENV:			// Shallow copy
      return shallow_copy(data);
    case ATOM:			// Copy depends on atm_should_cpy
      {
	rt_data_t *no_own = env_look_up(env, data->_atom.str);
	if (atm_should_cpy) return deep_copy(no_own);
	return no_own;
      }
    case LIST:			// Determine context and then eval
      /* It is an empty list */
      if (data->_list.size == 0) return shallow_copy(data);

      /* Look for special syntax *stuff* */
      if (data->_list.list[0]->tag == ATOM)
	{
	  const char *id = data->_list.list[0]->_atom.str;
	  if (strcmp("=", id) == 0) /* Define */
	    {
	      return proc_assignment(env, data, true);
	    }

	  if (strcmp("<-", id) == 0) /* Mutate */
	    {
	      return proc_assignment(env, data, false);
	    }

	  if (strcmp("&", id) == 0) /* quote */
	    {
	      if (data->_list.size == 0) return alloc_list(0);
	      return shallow_copy(data->_list.list[1]);
	    }

	  if (strcmp("->", id) == 0)  /* lambda */
	    {
	      if (data->_list.list[1]->tag == LIST)
		{
#define CPY_ENV(m) (&shallow_copy((rt_data_t *)m)->_env)
		  rt_env_t *env_cpy = CPY_ENV(env);
		  rt_data_t *expr_body = shallow_copy(data->_list.list[2]);
		  rt_list_t *params = &data->_list.list[1]->_list;
		  if (params->size == 0)
		    {
		      return (rt_data_t *)
			make_clos_func(new_mp_env(env_cpy), NULL, expr_body);
		    }

		  size_t i;
		  for (i = params->size; i > 0; --i)
		    {
		      expr_body = (rt_data_t *)
			make_clos_func(env_cpy,
				       params->list[i - 1]->_atom.str,
				       expr_body);
		      env_cpy = CPY_ENV(env_cpy);
		    }
		  /* Deallocate that extra-copied env_cpy */
		  rt_data_t *dr = (rt_data_t *) env_cpy;
		  dealloc(&dr);
		  return expr_body;
#undef CPY_ENV
		}
	      return from_err_msg("EXPECT LIST FOR PARAMETERS -- ->");
	    }

	  if (strcmp("and", id) == 0) /* short-circuit and */
	    {
	      /* GUARD macros do not work here! */
	      rt_data_t *ret = NULL;
	      size_t i;
	      for (i = 1; i < data->_list.size; ++i)
		{
		  dealloc(&ret);
		  ret = eval(env, data->_list.list[i]);
		  if (!ret) return from_err_msg("TYPE IS NULL -- and");
		  if (!expr_is_truthy(ret)) break;
		}
	      return ret;
	    }

	  if (strcmp("or", id) == 0) /* short-circuit or */
	    {
	      /* GUARD macros do not work here! */
	      rt_data_t *ret = NULL;
	      size_t i;
	      for (i = 1; i < data->_list.size; ++i)
		{
		  dealloc(&ret);
		  ret = eval(env, data->_list.list[i]);
		  if (ret && (ret->tag == ERR || expr_is_truthy(ret))) break;
		}
	      if (!ret) return from_err_msg("TYPE IS NULL -- or");
	      return ret;
	    }

	  if (strcmp("do", id) == 0) /* Do-end block */
	    {
	      /* GUARD macros do not work here! */
	      rt_data_t *ret = NULL;
	      size_t i;
	      for (i = 1; i < data->_list.size; ++i)
		{
		  dealloc(&ret);
		  ret = eval(env, data->_list.list[i]);
		  if (ret && ret->tag == ERR) break;
		}
	      if (!ret) return from_err_msg("TYPE IS NULL -- do");
	      return ret;
	    }

	  if (strcmp("cond", id) == 0) /* If-else block */
	    {
	      size_t i;
	      for (i = 1; i < data->_list.size; ++i)
		{
		  rt_data_t *pnode = data->_list.list[i];
		  assert(pnode->tag == LIST);
		  assert(pnode->_list.size == 2);

		  /* Else is only valid within a cond */
		  if (pnode->_list.list[0]->tag == ATOM
		      && strcmp("else", pnode->_list.list[0]->_atom.str) == 0)
		    {
		      return eval(env, pnode->_list.list[1]);
		    }

		  rt_data_t *pred = eval(env, pnode->_list.list[0]);
		  if (!pred) return from_err_msg("TYPE IS NULL -- cond");
		  if (pred->tag == ERR) return pred;
		  if (expr_is_truthy(pred))
		    {
		      dealloc(&pred);
		      return eval(env, pnode->_list.list[1]);
		    }
		}
	      return from_err_msg("NOT ALL CASES WERE HANDLED -- cond");
	    }
	}

      /* Default strategy: apply */
      switch (data->_list.size)
	{
	case 1:			// Parameter-less function call
	  {
	    rt_data_t *ret = NULL;
	    GUARD_DO(base, eval(env, data->_list.list[0]), g1);

	    switch (base->tag) // base is guaranteed to not be ERR
	      {
	      case FUNC:
		ret = base->_func.fptr(env, NULL, NULL);
		break;
	      case UDT:
		ret = base->_udt.apply(base, env, NULL);
		break;
	      default:
		ret = from_err_msg("TYPE IS NOT APPLIABLE -- APPLY");
		break;
	      }
	    GUARD_END(base, g1);
	    return ret;
	  }
	case 2:			// Function call or Unary operator
	  {
	    rt_data_t *ret = NULL;
	    GUARD_DO(base, eval(env, data->_list.list[0]), g2);
	    GUARD_DO(p1, eval(env, data->_list.list[1]), g2);

	    switch (base->tag) // base is guaranteed to not be ERR
	      {
	      case FUNC:
		ret = base->_func.fptr(env, p1, NULL);
		break;
	      case UDT:
		ret = base->_udt.apply(base, env, p1);
		break;
	      default:
		ret = from_err_msg("TYPE IS NOT APPLIABLE -- APPLY");
		break;
	      }
	    GUARD_END(p1, g2);
	    GUARD_END(base, g2);
	    return ret;
	  }
	case 3:			// Binary operator
	  {
	    rt_data_t *ret = NULL;
	    GUARD_DO(base, eval(env, data->_list.list[0]), g3);
	    GUARD_DO(p1, eval(env, data->_list.list[1]), g3);
	    GUARD_DO(p2, eval(env, data->_list.list[2]), g3);

	    switch (base->tag) // base is guaranteed to not be ERR
	      {
	      case FUNC:
		ret = base->_func.fptr(env, p1, p2);
		break;
	      default:
		ret = from_err_msg("TYPE IS NOT APPLIABLE -- APPLY");
		break;
	      }
	    GUARD_END(p2, g3);
	    GUARD_END(p1, g3);
	    GUARD_END(base, g3);
	    return ret;
	  }
	default:
	  return from_err_msg("CANNOT INFER LIST CONTEXT -- EVAL");
	}
    default:
      return from_err_msg("UNRECOGNIZED DATA TAG -- EVAL");
    }
}

rt_data_t *
eval(rt_env_t *env, rt_data_t *data)
{
  // Deep copy should be performed upon seeing an atom
  return intern_eval(env, data, true);
}

#undef GUARD_END
#undef GUARD_DO
