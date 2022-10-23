cmake_minimum_required(VERSION 3.12)

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

add_library(pico_pdm_microphone INTERFACE)

target_sources(pico_pdm_microphone INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/pdm_microphone.c
    ${CMAKE_CURRENT_LIST_DIR}/src/OpenPDM2PCM/OpenPDMFilter.c
)

target_include_directories(pico_pdm_microphone INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)

pico_generate_pio_header(pico_pdm_microphone ${CMAKE_CURRENT_LIST_DIR}/src/pdm_microphone.pio)

target_link_libraries(pico_pdm_microphone INTERFACE pico_stdlib hardware_dma hardware_pio)


add_library(ggwave INTERFACE)

target_sources(ggwave INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ggwave/src/ggwave.cpp
)

target_include_directories(ggwave INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ggwave/include
)

add_executable(ggtag
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

add_library(epd INTERFACE)
target_sources(epd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/DEV_Config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/EPD_2in13_V3.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/GUI_Paint.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font8.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font12.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font16.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font20.c
    ${CMAKE_CURRENT_LIST_DIR}/src/epd/font24.c
)
target_include_directories(epd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/epd
)

target_link_libraries(ggtag PUBLIC pico_pdm_microphone ggwave epd hardware_spi hardware_sleep)

# enable usb output, disable uart output
pico_enable_stdio_usb(ggtag 1)
pico_enable_stdio_uart(ggtag 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(ggtag)
