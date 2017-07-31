#include "eval.h"

rt_data_t *
eval(rt_env_t *env, rt_data_t *data)
{
  if (!data) return NULL;
  switch (data->tag)
    {
    case CHAR:
    case INT:
    case FLOAT:
    case FUNC:
    case ERR:
    case ENV:			// Shallow copy
      return shallow_copy(data);
    case ATOM:			// Deep clone
      return deep_copy(env_look_up(env, data->_atom.str));
    case LIST:			// Determine context and then eval
#define GUARD_DO(name, value, id)				\
      rt_data_t *name = value;					\
      if (!name)						\
	{							\
	  ret = from_err_msg("TYPE IS NULL -- EVAL");		\
	  goto name## _ ##id;					\
	}							\
      if (name->tag == ERR)					\
	{							\
	  ret = name;						\
	  goto name## _ ##id;					\
	}							\
      {

#define GUARD_END(name, id)			\
      dealloc(&name);				\
    }						\
      name## _ ##id:

      switch (data->_list.size)
	{
	case 0:			// It is an empty list
	  return shallow_copy(data);
	case 1:			// Parameter-less function call
	  {
	    rt_data_t *ret = NULL;
	    GUARD_DO(base, eval(env, data->_list.list[0]), g1);

	    rt_data_t *env_cpy = shallow_copy((rt_data_t*) env);
	    switch (base->tag) // base is guaranteed to not be ERR
	      {
	      case FUNC:
		ret = base->_func.fptr(&env_cpy->_env, NULL, NULL);
		break;
	      default:
		ret = from_err_msg("TYPE IS NOT APPLIABLE -- EVAL");
		break;
	      }
	    dealloc(&env_cpy);
	    GUARD_END(base, g1);
	    return ret;
	  }
	case 2:			// Function call or Unary operator
	  {
	    rt_data_t *ret = NULL;
	    GUARD_DO(base, eval(env, data->_list.list[0]), g2);
	    GUARD_DO(p1, eval(env, data->_list.list[1]), g2);

	    rt_data_t *env_cpy = shallow_copy((rt_data_t*) env);
	    switch (base->tag) // base is guaranteed to not be ERR
	      {
	      case FUNC:
		ret = base->_func.fptr(&env_cpy->_env, p1, NULL);
		break;
	      default:
		ret = from_err_msg("TYPE IS NOT APPLIABLE -- EVAL");
		break;
	      }
	    dealloc(&env_cpy);
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

	    rt_data_t *env_cpy = shallow_copy((rt_data_t*) env);
	    switch (base->tag) // base is guaranteed to not be ERR
	      {
	      case FUNC:
		ret = base->_func.fptr(&env_cpy->_env, p1, p2);
		break;
	      default:
		ret = from_err_msg("TYPE IS NOT APPLIABLE -- EVAL");
		break;
	      }
	    dealloc(&env_cpy);
	    GUARD_END(p2, g3);
	    GUARD_END(p1, g3);
	    GUARD_END(base, g3);
	    return ret;
	  }
	default:
	  return from_err_msg("CANNOT INFER LIST CONTEXT -- EVAL");
	}
#undef GUARD_END
#undef GUARD_DO
    default:
      return from_err_msg("UNRECOGNIZED DATA TAG -- EVAL");
    }
}
