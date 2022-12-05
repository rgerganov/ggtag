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

set(EPD "waveshare" CACHE STRING "2.13inch waveshare EPD")

# initialize the Pico SDK
pico_sdk_init()

#
# pico_pdm_microphone
#

set(TARGET pico_pdm_microphone)

add_library(${TARGET} INTERFACE)

target_sources(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/target/src/pdm_microphone.c
    ${CMAKE_CURRENT_LIST_DIR}/target/src/OpenPDMFilter.c
)

target_include_directories(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/target/include
    ${CMAKE_CURRENT_LIST_DIR}/shared/include
)

pico_generate_pio_header(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/target/src/pdm_microphone.pio)

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
    ${CMAKE_CURRENT_LIST_DIR}/target/src/DEV_Config.c
    ${CMAKE_CURRENT_LIST_DIR}/shared/src/GUI_Paint.c
    ${CMAKE_CURRENT_LIST_DIR}/shared/src/protocol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shared/src/font8.c
    ${CMAKE_CURRENT_LIST_DIR}/shared/src/font12.c
    ${CMAKE_CURRENT_LIST_DIR}/shared/src/font16.c
    ${CMAKE_CURRENT_LIST_DIR}/shared/src/font20.c
    ${CMAKE_CURRENT_LIST_DIR}/shared/src/font24.c
)
if (EPD STREQUAL "waveshare")
    message("Using waveshare e-paper display")
    target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/target/src/EPD_2in13_V3.c
    )
elseif (EPD STREQUAL "rhink")
    message("Using rhink e-paper display")
    target_sources(${TARGET} INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/target/src/EPD_2in13_rhink.c
    )
else()
    message(FATAL_ERROR "Unsupported EPD: ${EPD}")
endif()
target_include_directories(${TARGET} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/target/include
)

#
# ggtag
#

set(TARGET ggtag)

add_executable(${TARGET}
    ${CMAKE_CURRENT_LIST_DIR}/target/src/main.cpp
)

target_link_libraries(${TARGET} PUBLIC pico_pdm_microphone ggwave epd hardware_spi hardware_sleep)

# deep-sleep mode
# 0 - no deep-sleep, stdio works
# 1 - RTC deep-sleep, stdio doesn't work
# 2 - GPIO deep-sleep, stdio doesn't work
set(GGTAG_DEEP_SLEEP 2)

target_compile_definitions(${TARGET} PUBLIC -DGGTAG_DEEP_SLEEP=${GGTAG_DEEP_SLEEP})

# link both usb and uart and choose one of them in runtime:
#   if running from USB then use usb output
#   if running from battery then use uart output
pico_enable_stdio_usb (${TARGET} 1)
pico_enable_stdio_uart(${TARGET} 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${TARGET})

#
# sleep_test0
#

set(TARGET sleep_test0)

add_executable(${TARGET}
    ${CMAKE_CURRENT_LIST_DIR}/target/src/sleep_test0.cpp
)

target_link_libraries(${TARGET} PUBLIC pico_stdlib hardware_sleep)

# enable usb output, disable uart output
pico_enable_stdio_usb (${TARGET} 1)
pico_enable_stdio_uart(${TARGET} 0)

pico_add_extra_outputs(${TARGET})
