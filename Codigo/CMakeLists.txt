cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(pico_sdk_import.cmake)
project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_library(dht INTERFACE)

pico_generate_pio_header(dht ${CMAKE_CURRENT_LIST_DIR}/dht.pio)

add_library(libs 
            libs/dht.c 
            libs/lcd_i2c.c)

target_include_directories(libs PUBLIC
    libs/inc
)

add_executable(${PROJECT_NAME} main.c)

target_include_directories(dht
    INTERFACE
    ./include)

target_sources(dht
    INTERFACE
    dht.c
)

target_link_libraries(${PROJECT_NAME} pico_stdlib pico_time hardware_adc hardware_pwm libs)

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

target_link_libraries(libs
    pico_stdlib
    hardware_adc
    hardware_pwm
    hardware_pio
    hardware_dma
    hardware_i2c
)

target_link_libraries(dht
    INTERFACE
    hardware_clocks
    hardware_dma
    hardware_pio
    hardware_i2c
)