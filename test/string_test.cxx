#include "catch.hpp"

#include "rt_data.h"

#include <string>

SCENARIO("str behaviour", "[str]") {
  GIVEN("an str storing hello") {
    std::string raw = "hello";
    rt_data_t *str = from_string(raw.c_str());

    REQUIRE(str->tag == STR);
    REQUIRE(raw.length() == str->_atom.size);
    REQUIRE(raw == str->_atom.str);

    WHEN("the original string changes") {
      raw[0] = 'b';
      THEN("the str does not change") {
	REQUIRE(str->_atom.str[0] == 'h');
      }
    }

    WHEN("the str is shallow copied") {
      size_t prev_refs = str->_atom.refs;
      rt_data_t *shcpy = shallow_copy(str);
      THEN("the string is not copied, and refs is increase by 1") {
	REQUIRE(str->_atom.str == shcpy->_atom.str);
	REQUIRE(str->_atom.refs == prev_refs + 1);
      }
      dealloc(&shcpy);
    }

    WHEN("the str is deep copied") {
      size_t prev_refs = str->_atom.refs;
      rt_data_t *dcpy = deep_copy(str);
      THEN("the string is copied, and refs does not change") {
	REQUIRE(str->_atom.str != dcpy->_atom.str);
	REQUIRE(str->_atom.refs == prev_refs);
      }
      dealloc(&dcpy);
    }

    dealloc(&str);
  }
}
