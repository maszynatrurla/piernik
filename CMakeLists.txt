cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(piernik C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

include(FreeRTOS_Kernel_import.cmake)

# rest of your project
add_executable(piernik
	src/main.c
	src/wolty.c
	src/power_save.c
	src/takie_czasy.c
	src/plaja.c
	src/httpserver.c
	src/lights.c
	src/effects.c
)

target_compile_options(piernik
  PRIVATE
    -O0
    -g
  )

target_compile_definitions(piernik PRIVATE
    NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
)

target_include_directories(piernik PRIVATE ${CMAKE_CURRENT_LIST_DIR} src)

pico_enable_stdio_usb(piernik 1)
pico_enable_stdio_uart(piernik 1)

target_link_libraries(piernik
    pico_cyw43_arch_lwip_sys_freertos
    pico_stdlib
    pico_lwip_iperf
    FreeRTOS-Kernel-Heap4
    hardware_pwm hardware_adc
    hardware_rtc hardware_uart
    hardware_timer)

pico_add_extra_outputs(piernik)
