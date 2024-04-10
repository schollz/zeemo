# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(zeemo C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(zeemo main.c)


target_link_libraries(zeemo
        pico_stdlib 
        hardware_i2c
        hardware_uart
)

pico_set_program_name(zeemo "zeemo")
pico_set_program_version(zeemo "0.1")

pico_enable_stdio_uart(zeemo 0)
pico_enable_stdio_usb(zeemo 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(zeemo)

