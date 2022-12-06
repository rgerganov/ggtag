# Overview

`ggtag` is an electronic tag that can be programmed with sound using the [ggwave](https://github.com/ggerganov/ggwave) library.
It is powered by the RP2040 microcontroller and uses Waveshare compatible e-paper display.
Programming over USB Serial port is also supported.

# Usage

TBD

# Building

## Firmware

Follow these steps to build the RP2040 firmware:

```
git clone https://github.com/raspberrypi/pico-sdk
cd pico-sdk
git submodule init
git submodule update
cd ..
git clone https://github.com/raspberrypi/pico-extras
cd pico-extra
git submodule init
git submodule update
cd ..
export PICO_SDK_PATH=<full_path_to_pico_sdk>
export PICO_EXTRAS_PATH=<full_path_to_pico_extras>
export PICO_TOOLCHAIN_PATH=<full_path_to_arm_toolchain>

git clone git@github.com:rgerganov/ggtag
cd ggtag
git submodule init
git submodule update
mkdir build
cd build
cmake .. -DPICO_BOARD=pico
make
```

## Web interface

[Emscripten](https://emscripten.org/) is used for building the web interface:

```
source <path_to_emsdk_env.sh>
CXX=emcc make
```

# Flashing the firmware

Press and hold the BOOTSEL button while putting the battery. Copy `build/ggtag.uf2` to the `RPI-RP2` drive. The tag will reboot and run the new firmware.

