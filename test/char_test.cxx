#include "catch.hpp"

#include "rt_data.h"

SCENARIO("char behaviour", "[char]") {
  GIVEN("a char storing 'a'") {
    rt_data_t *vchr = from_char('a');

    REQUIRE(vchr->tag == CHAR);
    REQUIRE(vchr->_char.c == 'a');

    WHEN("the char is shallow copied") {
      rt_data_t *shcpy = shallow_copy(vchr);
      THEN("the number stored is still the same") {
	REQUIRE(vchr->_char.c == shcpy->_char.c);
      }
      dealloc(&shcpy);
    }

    WHEN("the char is deep copied") {
      rt_data_t *dpy = deep_copy(vchr);
      THEN("the number stored is still the same") {
	REQUIRE(vchr->_char.c == dpy->_char.c);
      }
      dealloc(&dpy);
    }

    dealloc(&vchr);
  }
}
