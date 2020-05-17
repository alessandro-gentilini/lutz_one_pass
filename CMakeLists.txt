cmake_minimum_required (VERSION 2.6)
project (lutzOnePass)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Add Version number
set (lutzOnePass_VERSION_MAJOR 1)
set (lutzOnePass_VERSION_MINOR 0)

# Set some flags necessary for the MAC OS builds
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
   set(CMAKE_MACOSX_RPATH 1)
endif()

# If the user supplied a prefix, set it
if (prefix)
   set (CMAKE_INSTALL_PREFIX ${prefix})
endif()

# Define where we want to put things
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set (HEADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build/include)

# Define some of the default compiler flags
set (CMAKE_CXX_FLAGS "-std=c++11 ${CMake_CXX_FLAGS}")

# Define the sub-directories for building in
add_subdirectory (src)
add_subdirectory (test)

#-------------------------------------------

# Define installation directories
install(DIRECTORY include/
    USE_SOURCE_PERMISSIONS
	DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    FILES_MATCHING PATTERN "lutz*.hpp")

install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	USE_SOURCE_PERMISSIONS
	DESTINATION ${CMAKE_INSTALL_PREFIX})

install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	USE_SOURCE_PERMISSIONS
	DESTINATION ${CMAKE_INSTALL_PREFIX})

#-------------------------------------------
