cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS "on")

# initialize pico_sdk from GIT
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(ggtag)

# initialize the Pico SDK
pico_sdk_init()

#
# pico_pdm_microphone
#

set(TARGET pico_pdm_microphone)

add_library(${TARGET} INTERFACE)

target_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/pdm_microphone.c
    ${CMAKE_CURRENT_LIST_DIR}/src/OpenPDM2PCM/OpenPDMFilter.c
)

target_include_directories(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)

pico_generate_pio_header(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/src/pdm_microphone.pio)

target_link_libraries(${TARGET} INTERFACE pico_stdlib hardware_dma hardware_pio)

#
# ggwave
#

set(TARGET ggwave)

add_library(${TARGET} INTERFACE)

target_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ggwave/src/ggwave.cpp
)

target_include_directories(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ggwave/include
)

#
# epd
#

set(TARGET epd)

add_library(${TARGET} INTERFACE)
target_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/DEV_Config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/EPD_2in13_V3.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/GUI_Paint.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font8.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font12.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font16.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font20.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font24.c
)
target_include_directories(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/epd
)

#
# ggtag
#

set(TARGET ggtag)

add_executable(${TARGET}
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

target_link_libraries(${TARGET} PUBLIC pico_pdm_microphone ggwave epd hardware_spi hardware_sleep)

# enable usb output, disable uart output
#pico_enable_stdio_usb (${TARGET} 1)
pico_enable_stdio_uart(${TARGET} 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${TARGET})

#
# sleep_test0
#

set(TARGET sleep_test0)

add_executable(${TARGET}
    ${CMAKE_CURRENT_LIST_DIR}/src/sleep_test0.cpp
)

target_link_libraries(${TARGET} PUBLIC pico_stdlib hardware_sleep)

# enable usb output, disable uart output
pico_enable_stdio_usb (${TARGET} 1)
pico_enable_stdio_uart(${TARGET} 0)

pico_add_extra_outputs(${TARGET})
