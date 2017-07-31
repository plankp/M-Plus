#include "catch.hpp"

#include "rt_data.h"

SCENARIO("float behaviour", "[float]") {
  GIVEN("a float storing 1.2") {
    rt_data_t *vflt = from_double(1.2);

    REQUIRE(vflt->tag == FLOAT);
    REQUIRE(vflt->_float.f == 1.2);

    WHEN("the float is shallow copied") {
      rt_data_t *shcpy = shallow_copy(vflt);
      THEN("the number stored is still the same") {
	REQUIRE(vflt->_float.f == shcpy->_float.f);
      }
      dealloc(&shcpy);
    }

    WHEN("the float is deep copied") {
      rt_data_t *dpy = deep_copy(vflt);
      THEN("the number stored is still the same") {
	REQUIRE(vflt->_float.f == dpy->_float.f);
      }
      dealloc(&dpy);
    }

    dealloc(&vflt);
  }
}
