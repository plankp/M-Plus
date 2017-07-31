cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0054 NEW)
project(MPLUS)

set(CATCH_INCLUDE_DIR ${MPLUS_SOURCE_DIR}/lib/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "-std=c99 -Wall -O3")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O3")
    set(CMAKE_EXE_LINKER_FLAGS "-Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections,--strip-all")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_C_FLAGS "-std=c99 -Wall -O3")
    set(CMAKE_CXX_FLAGS, "-std=c++11 -Wall -O2 -Wc++11-extensions")
    set(CMAKE_EXE_LINKER_FLAGS "-Os")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "/O2 /TC /Za /W4")
    set(CMAKE_CXX_FLAGS "/O2 /W4 /GR /EHsc")
    set(CMAKE_EXE_LINKER_FLAGS "/OPT:REF")
endif()

include_directories(${MPLUS_SOURCE_DIR}/header)
add_subdirectory(${MPLUS_SOURCE_DIR}/source)
add_subdirectory(${MPLUS_SOURCE_DIR}/test)

add_executable(mplus ${MPLUS_SOURCE_DIR}/main.cxx)
target_link_libraries(mplus mplus_lib)

add_executable(tests
    $<TARGET_OBJECTS:test_cases>
    ${MPLUS_SOURCE_DIR}/test_main.cxx)
target_link_libraries(tests mplus_lib)
target_link_libraries(tests Catch)

enable_testing()
add_test(all tests)
