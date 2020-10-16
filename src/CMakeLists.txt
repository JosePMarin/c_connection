add_executable(main
    main.cpp
)

# target_include_directories(main PUBLIC 
#     ${CMAKE_SOURCE_DIR}/external/asio-1.18.0/include}
# )

include_directories(
    ${CMAKE_SOURCE_DIR}/external/asio/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)