cmake_minimum_required(VERSION 3.13)

add_library(network STATIC)
target_sources(network PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/access_point.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dhcpserver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/webserver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mqtt_client.c
    )
target_include_directories(network 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${PICO_LWIP_CONTRIB_PATH}/apps/httpd
    )

target_link_libraries(network 
    PUBLIC
        pico_cyw43_arch_lwip_threadsafe_background
    PRIVATE 
        pico_lwip_http
        pico_lwip_mqtt
        pico_httpd_content
        mcu_internal
    )

pico_add_library(pico_httpd_content NOFLAG)
pico_set_lwip_httpd_content(pico_httpd_content INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/content/index.shtml
    ${CMAKE_CURRENT_LIST_DIR}/content/setup_ok.shtml
    ${CMAKE_CURRENT_LIST_DIR}/content/setup_err.shtml
    )