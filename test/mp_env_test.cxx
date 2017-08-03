#include "catch.hpp"

#include "mp_env.h"
#include "rt_data.h"

SCENARIO("MP Environment behaviour", "[mp_env_t]") {
  GIVEN("an empty environment") {
    // This is static, otherwise it gets re-initialized
    // for every WHEN clause
    static rt_env_t *env = new_mp_env(nullptr);

    REQUIRE(env->tag == ENV);

    WHEN("look_up is used on a non-existant key") {
      THEN("ERR is returned") {
	auto ret = env_look_up(env, "z");
	REQUIRE(ret->tag == ERR);
	dealloc(&ret);
      }
    }

    AND_WHEN("define is used on a non-existant key") {
      rt_data_t *tmp = from_char('b');
      env_define(env, "a", tmp);

      THEN("it will be emplaced into the environment") {
	rt_data_t *emb = deep_copy(env_look_up(env, "a"));
	REQUIRE(tmp->tag == emb->tag);
	REQUIRE(tmp->_char.c == emb->_char.c);
       	dealloc(&emb);
      }
      AND_THEN("clean up locals should not remove from environment") {
	dealloc(&tmp);
	REQUIRE(env_look_up(env, "a") != nullptr);
      }
    }

    AND_WHEN("mutate is used on a non-existant key") {
      rt_data_t *tmp = from_char('b');
      env_mutate(env, "b", tmp);
      THEN("nothing will happen") {
	auto ret = env_look_up(env, "b");
	REQUIRE(ret->tag == ERR);
	dealloc(&ret);
      }
      AND_THEN("clean up locals") {
	dealloc(&tmp);
      }
    }

    AND_WHEN("mutate is used on an existant key") {
      rt_data_t *tmp = from_char('a');
      env_mutate(env, "a", tmp);
      THEN("the original value will be replaced") {
	rt_data_t *emb = deep_copy(env_look_up(env, "a"));
	REQUIRE(tmp->tag == emb->tag);
	REQUIRE(tmp->_char.c == emb->_char.c);
	dealloc(&emb);
      }
      AND_THEN("clean up locals") {
	dealloc(&tmp);
      }
    }

    AND_WHEN("remove is used on an existant key") {
      rt_data_t *tmp = env_look_up(env, "a");
      REQUIRE(tmp != nullptr);

      env_remove(env, "a");
      THEN("the value will be removed") {
	REQUIRE(env_look_up(env, "a") == nullptr);
      }
      // ownership of tmp is not transfered, do not dealloc
    }

    AND_WHEN("test ends") {
      THEN("clean up environment") {
	rt_data_t *denv = reinterpret_cast<rt_data_t*>(env);
	dealloc(&denv);
      }
    }
  }
}
