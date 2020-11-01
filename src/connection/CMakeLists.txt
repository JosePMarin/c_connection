# Declare the libraries
add_library(libconnection STATIC
        connection.cpp
)

# Specify the include directories for the headers exported by this library
target_include_directories(libconnection PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/asio/asio/include
)