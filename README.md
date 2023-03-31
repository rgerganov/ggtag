[![Build Status](https://github.com/rgerganov/ggtag/workflows/CI/badge.svg)](https://github.com/rgerganov/ggtag/actions)
# Overview
![logo](/docs/ggtag-192x192.png)

`ggtag` is an electronic tag that can be programmed with sound using the [ggwave](https://github.com/ggerganov/ggwave) library.
It is powered by the RP2040 microcontroller and uses Waveshare compatible e-paper display.
Additionally, ggtag supports USB serial programming and allows emulation of 125kHz RFID tags.

The firmware is open source and licensed under the GPLv3 license. The PCB is designed by [Eurolan](http://www.eurolan.net/).

# Usage

TBD

# Credits

* [ggwave](https://github.com/ggerganov/ggwave) (MIT license)
* [microphone-library-for-pico](https://github.com/ArmDeveloperEcosystem/microphone-library-for-pico/) (Apache 2.0 license)
* [Pico_ePaper_Code](https://github.com/waveshare/Pico_ePaper_Code) (GPLv3 license)
* [QR-Code-generator](https://github.com/nayuki/QR-Code-generator) (MIT license)
* [Serial API Polyfill](https://github.com/google/web-serial-polyfill) (Apache 2.0 license)
* [avrfid](https://github.com/scanlime/navi-misc/blob/master/avrfid/avrfid.S) (BSD license)

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

Live version of the web interface is available at [https://ggtag.io](https://ggtag.io)

## Flashing the firmware

Press and hold the BOOTSEL button while putting the battery. Copy `build/ggtag.uf2` to the `RPI-RP2` drive. The tag will reboot and run the new firmware.

