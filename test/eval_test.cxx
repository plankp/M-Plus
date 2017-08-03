#include "catch.hpp"

#include "eval.h"
#include "mp_env.h"
#include "rt_data.h"
#include "rt_utils.h"

static
rt_env_t *
make_prim_env (void)
{
  rt_env_t *env = new_mp_env(nullptr);
  init_default_env(env);
  return env;
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
