#include "catch.hpp"

#include "rt_data.h"

SCENARIO("int behaviour", "[int]") {
  GIVEN("an int storing 10") {
    rt_data_t *vint = from_long_long(10);

    REQUIRE(vint->tag == INT);
    REQUIRE(vint->_int.i == 10);

    WHEN("the int is shallow copied") {
      rt_data_t *shcpy = shallow_copy(vint);
      THEN("the number stored is still the same") {
	REQUIRE(vint->_int.i == shcpy->_int.i);
      }
      dealloc(&shcpy);
    }

    WHEN("the int is deep copied") {
      rt_data_t *dpy = deep_copy(vint);
      THEN("the number stored is still the same") {
	REQUIRE(vint->_int.i == dpy->_int.i);
      }
      dealloc(&dpy);
    }

    dealloc(&vint);
  }
}
