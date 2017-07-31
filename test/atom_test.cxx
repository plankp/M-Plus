#include "catch.hpp"

#include "rt_data.h"

#include <string>

SCENARIO("atom behaviour", "[atom]") {
  GIVEN("an atom storing hello") {
    std::string raw = "hello";
    rt_data_t *atom = from_string(raw.c_str());

    REQUIRE(atom->tag == ATOM);
    REQUIRE(raw.length() == atom->_atom.size);
    REQUIRE(raw == atom->_atom.str);

    WHEN("the original string changes") {
      raw[0] = 'b';
      THEN("the atom does not change") {
	REQUIRE(atom->_atom.str[0] == 'h');
      }
    }

    WHEN("the atom is shallow copied") {
      size_t prev_refs = atom->_atom.refs;
      rt_data_t *shcpy = shallow_copy(atom);
      THEN("the string is not copied, and refs is increase by 1") {
	REQUIRE(atom->_atom.str == shcpy->_atom.str);
	REQUIRE(atom->_atom.refs == prev_refs + 1);
      }
      dealloc(&shcpy);
    }

    WHEN("the atom is deep copied") {
      size_t prev_refs = atom->_atom.refs;
      rt_data_t *dcpy = deep_copy(atom);
      THEN("the string is copied, and refs does not change") {
	REQUIRE(atom->_atom.str != dcpy->_atom.str);
	REQUIRE(atom->_atom.refs == prev_refs);
      }
      dealloc(&dcpy);
    }

    dealloc(&atom);
  }
}
