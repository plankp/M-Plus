file(GLOB SOURCES "*.cxx")
add_library(test_cases OBJECT ${SOURCES})
target_include_directories(test_cases PUBLIC ${CATCH_INCLUDE_DIR})
