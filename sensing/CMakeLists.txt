cmake_minimum_required(VERSION 3.13)

add_library(sensing STATIC)
target_sources(sensing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BME68x_SensorAPI/bme68x.c
    )
target_include_directories(sensing 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/BME68x_SensorAPI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
target_link_libraries(sensing PRIVATE
    pico_stdlib
    hardware_i2c
    )