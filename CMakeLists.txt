# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
include(pico_extras_import.cmake)


# Set name of project (as PROJECT_NAME) and C/C++ standards
project(_core C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(${PROJECT_NAME} 
        main.c
        # ${CMAKE_CURRENT_LIST_DIR}/lib/usb_descriptors.c
)


# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/lib/WS2812.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/lib/buttonmatrix3.pio)

target_link_libraries(${PROJECT_NAME}
        pico_stdlib 
        hardware_clocks
        hardware_adc
        hardware_i2c
        hardware_pio
        hardware_uart
        # tinyusb_device
        # tinyusb_board
)

add_compile_definitions(USE_PRINTF)

set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "-Wl,--print-memory-usage")

# target_link_libraries(${PROJECT_NAME} 
#     tinyusb_device
#     tinyusb_board
# )
# pico_enable_stdio_usb(${PROJECT_NAME} 0)
# pico_enable_stdio_uart(${PROJECT_NAME} 1)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_enable_stdio_usb(${PROJECT_NAME} 1)




pico_add_extra_outputs(${PROJECT_NAME})
