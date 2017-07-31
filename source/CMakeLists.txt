add_subdirectory(crt)

file(GLOB SOURCES "*.cxx")
add_library(mplus_lib
    $<TARGET_OBJECTS:crt>
    ${SOURCES})
