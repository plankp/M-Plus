#include "eval.h"

#define GUARD_DO(name, value, id)				\
  rt_data_t *name = value;					\
  if (!name)							\
    {								\
      ret = from_err_msg("TYPE IS NULL -- eval");		\
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
      ret = deep_copy(rhs);
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
intern_eval(rt_env_t *env, rt_data_t *data, const bool should_cpy)
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
    case ATOM:			// Copy depends on should_cpy
      {
	rt_data_t *no_own = env_look_up(env, data->_atom.str);

	/* Returns regardless of should_cpy if it is an error */
	if (no_own->tag == ERR) return no_own;

	if (should_cpy) return deep_copy(no_own);
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
		  rt_env_t *env_cpy =
		    new_mp_env(&shallow_copy((rt_data_t *) env)->_env);
		  rt_data_t *expr_body = shallow_copy(data->_list.list[2]);
		  rt_list_t *params = &data->_list.list[1]->_list;
		  if (params->size == 0)
		    {
		      return (rt_data_t *)
			make_clos_func(env_cpy, NULL, expr_body);
		    }

		  size_t i;
		  for (i = params->size; i > 0; --i)
		    {
		      expr_body = (rt_data_t *)
			make_clos_func(env_cpy,
				       params->list[i - 1]->_atom.str,
				       expr_body);
		      env_cpy = &shallow_copy((rt_data_t *) env_cpy)->_env;
		    }
		  /* Deallocate that extra-copied env_cpy */
		  rt_data_t *dr = (rt_data_t *) env_cpy;
		  dealloc(&dr);
		  return expr_body;
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

	  /* Early returns in the code causes Warning C4702
	   * when it should not. The code is perfectly
	   * reachable (since there are gotos to make it
	   * work).
	   */
#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif
	  if (strcmp("try", id) == 0) /* try */
	    {
	      rt_data_t *ret = NULL;
	      GUARD_DO(body, eval(env, data->_list.list[1]), try);
	      /* Here means try did not throw error. return it */
	      return body;
	      GUARD_END(body, try);

	      /* Here means try did throw error. */
	      if (data->_list.list[2]->tag == LIST)
		{
		  if (data->_list.list[2]->_list.list[0]->tag == ATOM)
		    {
		      env_define(env,
				 data->_list.list[2]->_list.list[0]->_atom.str,
				 ret->_err.content);
		    }
		  GUARD_DO(cbody, eval(env, data->_list.list[2]->_list.list[1]),
			   catch);
		  return cbody;
		  GUARD_END(cbody, catch);
		}
	      return ret;
	    }
#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(pop)
#endif

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
		  const bool pred_rst = expr_is_truthy(pred);
		  dealloc(&pred);
		  if (pred_rst) return eval(env, pnode->_list.list[1]);
		}
	      return from_err_msg("NOT ALL CASES WERE HANDLED -- cond");
	    }
	}

      rt_data_t *ret = NULL;
      GUARD_DO(base, eval(env, data->_list.list[0]), apply);
      rt_data_t *params = alloc_list(data->_list.size - 1);
      size_t i;
      for (i = 0; i < params->_list.size; ++i)
	{
	  ret = eval(env, data->_list.list[i + 1]);
	  if (!ret)
	    {
	      ret = from_err_msg("TYPE IS NULL -- init apply");
	      goto param_error_jmp;
	    }
	  if (ret->tag == ERR) goto param_error_jmp;
	  params->_list.list[i] = ret;
	}
      ret = apply(env, base, &params->_list);
    param_error_jmp:
      dealloc(&params);
      GUARD_END(base, apply);
      return ret;
    default:
      return from_err_msg("UNRECOGNIZED DATA TAG -- eval");
    }
}

rt_data_t *
eval(rt_env_t *env, rt_data_t *data)
{
  // Deep copy should be performed upon seeing an atom
  return intern_eval(env, data, true);
}

rt_data_t *
apply(rt_env_t *env, rt_data_t *val, rt_list_t *param)
{
  switch (val->tag)
    {
    case FUNC:
      if (param == NULL || param->size == 0)
	{ return val->_func.fptr(env, NULL, NULL); }
      if (param->size == 1)
	{ return val->_func.fptr(env, param->list[0], NULL); }
      if (param->size == 2)
	{ return val->_func.fptr(env, param->list[0], param->list[1]); }
      return from_err_msg("CANNOT INFER LIST CONTEXT -- apply");
    case UDT:
      if (!val->_udt.apply) break;

      if (param == NULL || param->size == 0)
	{ return val->_udt.apply(val, env, NULL); }
      if (param->size == 1)
	{ return val->_udt.apply(val, env, param->list[0]); }
      return from_err_msg("CANNOT INFER LIST CONTEXT -- apply");
    case LIST:
      if (param == NULL || param->size == 0)
	{ return eval(env, val); }
      return from_err_msg("ILLEGAL LIST APPLICATION -- apply");
    default:
      break;
    }
  return from_err_msg("TYPE IS NOT APPLIABLE -- apply");
}

#undef GUARD_END
#undef GUARD_DO
