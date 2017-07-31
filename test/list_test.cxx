#include "catch.hpp"

#include "rt_data.h"

SCENARIO("list behaviour", "[list]") {
  GIVEN("a list storing 1, 'a', \"a\"") {
    rt_data_t *raw[3] = { from_long_long(1),
			  from_char('a'),
			  from_string("a") };
    rt_data_t *list = from_list(3, raw);

    REQUIRE(list->tag == LIST);
    REQUIRE(list->_list.size == 3);

    // Deallocate raw array before all tests to test ownership
    dealloc(&raw[0]);
    dealloc(&raw[1]);
    dealloc(&raw[2]);

    WHEN("the source is deallocated, the value stays in the list") {
      REQUIRE(list->_list.list[0]->tag == INT);
      REQUIRE(list->_list.list[0]->_int.i == 1);

      REQUIRE(list->_list.list[1]->tag == CHAR);
      REQUIRE(list->_list.list[1]->_char.c == 'a');

      // This array holds exclusive ownership of the atom: refs == 1
      REQUIRE(list->_list.list[2]->tag == STR);
      REQUIRE(list->_list.list[2]->_atom.refs == 1);
      REQUIRE(list->_list.list[2]->_atom.size == 1);
      REQUIRE(strcmp("a", list->_list.list[2]->_atom.str) == 0);
    }

    WHEN("the list is shallow copied") {
      size_t prev_atom_refs = list->_list.list[2]->_atom.refs;
      rt_data_t *shcpy = shallow_copy(list);
      THEN("refs of the elements are increased by 1, memory is different") {
	REQUIRE(list->_list.list != shcpy->_list.list);
	REQUIRE(list->_list.size == shcpy->_list.size);
	REQUIRE(list->_list.list[2]->_atom.refs == prev_atom_refs + 1);
      }
      dealloc(&shcpy);
    }

    WHEN("the list is deep copied") {
      size_t prev_atom_refs = list->_list.list[2]->_atom.refs;
      rt_data_t *dcpy = deep_copy(list);
      THEN("memory is copied, refs are not increased") {
	REQUIRE(list->_list.list != dcpy->_list.list);
	REQUIRE(list->_list.size == dcpy->_list.size);
	REQUIRE(list->_list.list[2]->_atom.refs == prev_atom_refs);
      }
      dealloc(&dcpy);
    }

    dealloc(&list);
  }
}
