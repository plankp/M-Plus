#include "catch.hpp"

#include "rt_data.h"

SCENARIO("array behaviour", "[array]") {
  GIVEN("a array storing 1, 'a', \"a\"") {
    rt_data_t *raw[3] = { from_int64(1),
			  from_char('a'),
			  from_string("a") };
    rt_data_t *array = from_array(3, raw);

    REQUIRE(array->tag == ARRAY);
    REQUIRE(array->_list.size == 3);

    // Deallocate raw array before all tests to test ownership
    dealloc(&raw[0]);
    dealloc(&raw[1]);
    dealloc(&raw[2]);

    WHEN("the source is deallocated, the value stays in the array") {
      REQUIRE(array->_list.list[0]->tag == INT);
      REQUIRE(array->_list.list[0]->_int.i == 1);

      REQUIRE(array->_list.list[1]->tag == CHAR);
      REQUIRE(array->_list.list[1]->_char.c == 'a');

      // This array holds exclusive ownership of the atom: refs == 1
      REQUIRE(array->_list.list[2]->tag == STR);
      REQUIRE(array->_list.list[2]->_atom.refs == 1);
      REQUIRE(array->_list.list[2]->_atom.size == 1);
      REQUIRE(strcmp("a", array->_list.list[2]->_atom.str) == 0);
    }

    WHEN("the array is shallow copied") {
      size_t prev_refs = array->_list.refs;
      size_t prev_atom_refs = array->_list.list[2]->_atom.refs;
      rt_data_t *shcpy = shallow_copy(array);
      THEN("refs of array is increased by 1, memory is same") {
	REQUIRE(array->_list.refs == prev_refs + 1);
      }
      AND_THEN("refs of elements do not change") {
	REQUIRE(array->_list.list[2]->_atom.refs == prev_atom_refs);
      }
      AND_THEN("memory is the same") {
	REQUIRE(array->_list.list == shcpy->_list.list);
	REQUIRE(array->_list.size == shcpy->_list.size);
      }
      dealloc(&shcpy);
    }

    WHEN("the array is deep copied") {
      size_t prev_atom_refs = array->_list.list[2]->_atom.refs;
      rt_data_t *dcpy = deep_copy(array);
      THEN("memory is copied, refs are not increased") {
	REQUIRE(array->_list.list != dcpy->_list.list);
	REQUIRE(array->_list.size == dcpy->_list.size);
	REQUIRE(array->_list.list[2]->_atom.refs == prev_atom_refs);
      }
      dealloc(&dcpy);
    }

    dealloc(&array);
  }
}
