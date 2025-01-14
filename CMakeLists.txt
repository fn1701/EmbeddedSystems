cmake_minimum_required(VERSION 3.10)

include(pico-sdk/pico_sdk_init.cmake)

project(ES VERSION 1.0)

pico_sdk_init()

include_directories(include)

add_executable(
    main
    src/main.c
)

target_link_libraries(main pico_stdlib hardware_pwm hardware_i2c)