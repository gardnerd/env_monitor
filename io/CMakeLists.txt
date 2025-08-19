cmake_minimum_required(VERSION 3.13)

add_library(io STATIC)
target_sources(io PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io.c
    )
target_include_directories(io 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
target_link_libraries(io PRIVATE 
    pico_stdlib
    )