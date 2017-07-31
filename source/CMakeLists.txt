add_subdirectory(syntree)
add_subdirectory(crt)
add_subdirectory(rt)

file(GLOB SOURCES "*.cxx")
add_library(mplus_lib
    $<TARGET_OBJECTS:syntree>
    $<TARGET_OBJECTS:crt>
    $<TARGET_OBJECTS:rt>
    ${SOURCES})
