cmake_minimum_required(VERSION 3.13)

add_library(webserver STATIC)
target_include_directories(webserver 
    PRIVATE 
        ${LWIP_OPTS_INCLUDE_DIR}
        ${PICO_LWIP_CONTRIB_PATH}/apps/httpd
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
target_sources(webserver PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/webserver.c
    )
target_link_libraries(webserver PRIVATE 
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_httpd_content
    )

pico_add_library(pico_httpd_content NOFLAG)
pico_set_lwip_httpd_content(pico_httpd_content INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/content/index.shtml
    ${CMAKE_CURRENT_LIST_DIR}/content/wifi_cred_ok.shtml
    ${CMAKE_CURRENT_LIST_DIR}/content/wifi_cred_err.shtml
    )