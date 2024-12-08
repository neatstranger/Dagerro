cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)



# Set the project name
project(fastForward)

# Initialize the Pico SDK
pico_sdk_init()

# Add your executable
add_executable(fastForward
    main.c
    i2c.c
)

target_include_directories(fastForward PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
# Link your executable with TinyUSB libraries and Pico SDK libraries
target_link_libraries(fastForward PUBLIC
    pico_stdlib
    hardware_i2c
)

# Enable USB functionality and disable UART if desired
pico_enable_stdio_usb(fastForward 0)
pico_enable_stdio_uart(fastForward 1)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(fastForward)
