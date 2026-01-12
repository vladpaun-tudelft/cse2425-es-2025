cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (defines pico_sdk_init(), pico_add_extra_outputs(), etc.)
include(pico_sdk_import.cmake)

project(robot C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(ROBOT_SRC_DIR "src" CACHE PATH "Source directory for robot sources")

add_executable(robot
    ${ROBOT_SRC_DIR}/main.c
    ${ROBOT_SRC_DIR}/timers.c
    ${ROBOT_SRC_DIR}/motors_pwm.c
    ${ROBOT_SRC_DIR}/HCSR04.c
)

pico_generate_pio_header(robot ${CMAKE_CURRENT_SOURCE_DIR}/${ROBOT_SRC_DIR}/HCSR04.pio)

target_link_libraries(robot
    pico_stdlib
    hardware_pio
)

pico_enable_stdio_usb(robot 1)
pico_enable_stdio_uart(robot 0)

pico_add_extra_outputs(robot)
