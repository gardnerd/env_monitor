cmake_minimum_required(VERSION 3.13)

add_library(mcu_internal STATIC)
target_sources(mcu_internal PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.c
    )
target_include_directories(mcu_internal 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
target_link_libraries(mcu_internal PRIVATE 
    pico_stdlib
    hardware_flash
    )