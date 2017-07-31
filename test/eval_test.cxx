#include "catch.hpp"

#include "eval.h"
#include "mp_env.h"
#include "rt_data.h"
#include "rt_utils.h"

#define PRIM_FUNCS				\
  DEF_GENERIC(add, +)				\
  DEF_GENERIC(sub, -)				\
  DEF_GENERIC(mul, *)				\
  DEF_GENERIC(div, /)

#define DEF_GENERIC(name, op)						\
  static								\
  rt_data_t *								\
  generic_##name(rt_env_t *env, rt_data_t *lhs, rt_data_t *rhs)		\
  {									\
    if (lhs->tag == rhs->tag && lhs->tag == INT)			\
      { return from_int64(lhs->_int.i op rhs->_int.i); }		\
    if (lhs->tag == rhs->tag && lhs->tag == FLOAT)			\
      { return from_int64(lhs->_float.f op rhs->_float.f); }	\
    return from_err_msg("ILLEGAL DATA TYPE -- " #op);			\
  }

PRIM_FUNCS

#undef DEF_GENERIC

static
rt_data_t *
generic_error(rt_env_t *env, rt_data_t *, rt_data_t *)
{
  return from_err_msg("ERROR");
}

static
rt_env_t *
make_prim_env (void)
{
  rt_env_t *env = new_mp_env();
#define DEF_GENERIC(name, op)				\
  do							\
    {							\
      rt_data_t *own = from_func_ptr(generic_##name);	\
      env_define(env, #op, own);			\
      dealloc(&own);					\
    }							\
  while(0);

PRIM_FUNCS;
DEF_GENERIC(error, error);

#undef DEF_GENERIC
  return env;
}

TEST_CASE("error behaviour", "[err]") {
  rt_data_t *err = generic_error(nullptr, nullptr, nullptr);
  REQUIRE(err->tag == ERR);
  dealloc(&err);
}

SCENARIO("eval behaviour", "[eval]") {
  GIVEN("a primitive environment") {
    static rt_env_t *env = make_prim_env();

    WHEN("eval nullptr") {
      THEN("nullptr is returned") {
	REQUIRE(eval(env, nullptr) == nullptr);
      }
    }

    WHEN("eval (+ 1 2)") {
      rt_data_t *exp = make_binary_expr(from_atom("+"),
					from_int64(1),
					from_int64(2));
      REQUIRE(exp->tag == LIST);
      REQUIRE(exp->_list.list[0]->tag == ATOM);
      REQUIRE(exp->_list.list[1]->tag == INT);
      REQUIRE(exp->_list.list[2]->tag == INT);

      THEN("answer should be 3") {
	rt_data_t *ret = eval(env, exp);
	REQUIRE(ret->tag == INT);
	REQUIRE(ret->_int.i == 3);
	dealloc(&ret);
      }
      dealloc(&exp);
    }

    AND_WHEN("eval (* (- 0 (+ 1 2)) 3)") {
      rt_data_t *exp =
	make_binary_expr(from_atom("*"),
			 make_binary_expr(from_atom("-"),
					  from_int64(0),
					  make_binary_expr(from_atom("+"),
							   from_int64(1),
							   from_int64(2))),
			 from_int64(3));
      REQUIRE(exp->tag == LIST);
      REQUIRE(exp->_list.list[0]->tag == ATOM);
      REQUIRE(exp->_list.list[1]->tag == LIST);
      REQUIRE(exp->_list.list[2]->tag == INT);

      THEN("answer should be -9") {
	rt_data_t *ret = eval(env, exp);
	REQUIRE(ret->tag == INT);
	REQUIRE(ret->_int.i == -9);
	dealloc(&ret);
      }
      dealloc(&exp);
    }

    AND_WHEN("eval (/ 3 (error))") {
      rt_data_t *exp =
        make_binary_expr(from_atom("/"),
			 from_int64(3),
			 make_nullary_expr(from_atom("error")));
      REQUIRE(exp->tag == LIST);
      REQUIRE(exp->_list.list[0]->tag == ATOM);
      REQUIRE(exp->_list.list[1]->tag == INT);
      REQUIRE(exp->_list.list[2]->tag == LIST);

      THEN("error is thrown") {
	rt_data_t *ret = eval(env, exp);
	REQUIRE(ret->tag == ERR);
	dealloc(&ret);
      }
      dealloc(&exp);
    }

    // Tear-down
    AND_WHEN("test ends") {
      THEN("clean up environment") {
	rt_data_t *denv = reinterpret_cast<rt_data_t*>(env);
	dealloc(&denv);
      }
    }
  }
}
