add_executable(main
    test.cpp
)

# target_include_directories(main PUBLIC 
#     ${CMAKE_SOURCE_DIR}/external/asio-1.18.0/include}
# )

include_directories(${CMAKE_SOURCE_DIR}/external/asio/asio/include)