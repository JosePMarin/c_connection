add_executable(main
    main.cpp
)

# target_include_directories(main PUBLIC 
#     ${CMAKE_SOURCE_DIR}/external/asio-1.18.0/include}
# )

target_link_libraries(main
    PUBLIC
    libconnection
)

add_subdirectory(connection)