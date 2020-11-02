cmake_minimum_required(VERSION 3.10.2)
cmake_policy(SET CMP0048 NEW)

project(c_connection VERSION 1.0.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set (CMAKE_INSTALL_TESTDIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests" CACHE STRING "Test executable (bin/tests)")

mark_as_advanced(CMAKE_INSTALL_TESTDIR)
set (CMAKE_INSTALL_BINDIR "${CMAKE_CURRENT_SOURCE_DIR}/bin" CACHE STRING "USER executables (bin)")
set (CMAKE_INSTALL_LIBDIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/lib${LIB_SUFFIX}" CACHE STRING "Object code libraries (lib)")
set (CMAKE_INSTALL_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/include" CACHE STRING "header files (include)")

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(src)
