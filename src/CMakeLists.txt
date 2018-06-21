
#------------------------------------------
# Define include directories
#------------------------------------------
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include)

#------------------------------------------
# Define the source and header files needed
# by the CppEphem library
#------------------------------------------
set (lutzop_SOURCES
    lutzObject.cpp
    lutzOnePass.cpp
    )

set (lutzop_HEADERS
    ../include/lutzObject.hpp
    ../include/lutzOnePass.hpp
    )

#------------------------------------------
# Create both a static and shared version of the library
#------------------------------------------
add_library (lutzop SHARED ${lutzop_SOURCES} ${lutzop_HEADERS})
add_library (lutzop_static STATIC ${lutzop_SOURCES} ${lutzop_HEADERS})

# Make sure the static version has the same name
set_target_properties(lutzop_static PROPERTIES OUTPUT_NAME lutzop)
