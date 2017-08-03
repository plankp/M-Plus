#include "catch.hpp"

#include "rt_data.h"
#include "rt_utils.h"

SCENARIO("Comparison behaviour", "[cmp]") {
  GIVEN("two characters") {
    rt_data_t *a = from_char('a');
    rt_data_t *b = from_char('b');

    REQUIRE(a->tag == CHAR);
    REQUIRE(b->tag == CHAR);
    REQUIRE(a->_char.c == 'a');
    REQUIRE(b->_char.c == 'b');

    WHEN("compared 'a', 'b'") {
      THEN("a negative number should be returned") {
	int ret;
	REQUIRE(expr_cmp(a, b, &ret));
	REQUIRE(ret < 0);
      }
    }

    dealloc(&a);
    dealloc(&b);
  }

  GIVEN("two atoms") {
    rt_data_t *a = from_atom("a");
    rt_data_t *b = from_atom("b");
    
    REQUIRE(a->tag == ATOM);
    REQUIRE(b->tag == ATOM);
    REQUIRE(strcmp(a->_atom.str, "a") == 0);
    REQUIRE(strcmp(b->_atom.str, "b") == 0);

    WHEN("compared b, a") {
      THEN("a positive number should be returned") {
	int ret;
	REQUIRE(expr_cmp(b, a, &ret));
	REQUIRE(ret > 0);
      }
    }

    dealloc(&a);
    dealloc(&b);
  }

  GIVEN("two lists") {
    rt_data_t *a = make_unary_expr(from_int64(3), from_int64(5));
    rt_data_t *b = make_nullary_expr(from_int64(10));

    WHEN("compared [3,5], [10]") {
      THEN("a negative number should be returned") {
	int ret;
	REQUIRE(expr_cmp(a, b, &ret));
	REQUIRE(ret < 0);
      }
    }

    dealloc(&a);
    dealloc(&b);
  }
}
