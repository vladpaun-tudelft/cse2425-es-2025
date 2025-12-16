cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (defines pico_sdk_init(), pico_add_extra_outputs(), etc.)
include(pico_sdk_import.cmake)

project(robot C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(robot
    src/main.c
)

target_link_libraries(robot
    pico_stdlib
)

# Optional but recommended for USB serial + easier workflow :contentReference[oaicite:1]{index=1}
pico_enable_stdio_usb(robot 1)
pico_enable_stdio_uart(robot 0)

pico_add_extra_outputs(robot)
