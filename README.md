[![Build Status](https://github.com/rgerganov/ggtag/workflows/CI/badge.svg)](https://github.com/rgerganov/ggtag/actions)
# Overview

`ggtag` is an electronic tag that can be programmed with sound using the [ggwave](https://github.com/ggerganov/ggwave) library.
It is powered by the RP2040 microcontroller and uses Waveshare compatible e-paper display.
Additionally, ggtag supports USB serial programming and allows emulation of 125kHz RFID tags.

<table>
  <tr>
    <td>
      <img src="/pics/front.jpg"></img>
    </td>
    <td>
      <img src="/pics/back.jpg"></img>
    </td>
  </tr>
</table>

Both firmware and hardware are open source and licensed under the GPLv3 license.
The PCB is made by [Eurolan](http://www.eurolan.net/), you can find more details about the hardware design in the [hardware](/hardware) directory.

# Usage

`ggtag` can be programmed either with sound or with USB serial.
To program with sound, you need to put a CR2032 battery in the battery holder and flip the switch to the `BATT` position.
To program with USB serial, you need to connect `ggtag` to a USB host and flip the switch to the `USB` position.
Both programming modes are supported in the web interface hosted at [ggtag.io](https://ggtag.io), all you need is a modern HTML browser.

## Programming with sound

Here is a demonstration of how programming with sound works. Initially, three ggtags are listening and then programmed simultaneously using mobile phone.
After the initial programming, you need to press the button on the side for ggtag to start listening again. At the end make sure to flip the switch back to `USB` position in order to preserve the battery.

https://user-images.githubusercontent.com/271616/233822561-fff766fd-9242-4774-a4a4-4f87c363f370.mp4

## Programming with USB serial

The web interface is using [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API) for USB serial programming, so you need a web browser (e.g. Chrome, Edge) which supports this API. Mobile Chrome running on Android is also supported. Here is a demo of using Pixel5 phone for USB serial programming:

https://user-images.githubusercontent.com/271616/233822572-c4ee6f94-ced6-48ab-8f59-7cc725db4bb0.mp4

## Emulating RFID tags

You can emulate 125kHz EM4102 RFID tags with ggtag. This is accomplished with an ATtiny85 MCU and the [avrfid](https://github.com/scanlime/navi-misc/blob/master/avrfid/avrfid.S) firmware. When ggtag receives an RFID command, it generates the corresponding avrfid firmware and programs the ATtiny85 with it. Currently, the avrfid firmware works only in passive mode, so you have to switch off the battery (flip the switch to `USB`) before using ggtag as RFID tag. Demo:

https://user-images.githubusercontent.com/271616/233822577-56f58177-3848-44f0-9bb0-32217d96b5e4.mp4

## Python examples

There is a `ggtag` python package which can be used together with `pyserial` or `ggwave` to program the device. See the [examples](examples/) folder for more details.

# Known issues

 * The RFID function works only when `ggtag` is in close proximity with the RFID reader. You may also need to hold it for several seconds for read to be successful.

# Credits

* [ggwave](https://github.com/ggerganov/ggwave) (MIT license)
* [microphone-library-for-pico](https://github.com/ArmDeveloperEcosystem/microphone-library-for-pico/) (Apache 2.0 license)
* [Pico_ePaper_Code](https://github.com/waveshare/Pico_ePaper_Code) (GPLv3 license)
* [QR-Code-generator](https://github.com/nayuki/QR-Code-generator) (MIT license)
* [Serial API Polyfill](https://github.com/google/web-serial-polyfill) (Apache 2.0 license)
* [avrfid](https://github.com/scanlime/navi-misc/blob/master/avrfid/avrfid.S) (BSD license)
* [stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h) (MIT license)
* [Font Awesome](https://fontawesome.com/) (CC BY 3.0 license)

# Project structure

The codebase is structured in the following way:
 * `target` - contains the firmware source code for RP2040
 * `shared` - contains the shared code between the firmware and the host library
 * `host` - contains the host library
 * `docs` - contains the web interface
 * `python` - contains Python bindings for the host library
 * `examples` - contains some examples
 * `hardware` - contains hardware design files

## Building the firmware

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

## Building the web interface

The host library is compiled to WASM using [Emscripten](https://emscripten.org/):
```
source <path_to_emsdk_env.sh>
CXX=emcc make
```
You can start a local server with `make server` and access the web interface at [http://localhost:8000](http://localhost:8000).
There is a hosted version available at [https://ggtag.io](https://ggtag.io)

## Flashing the firmware

Put ggtag into USB mode, press and hold the button and plug the USB cable.
Copy `build/ggtag-3in52-rse.uf2` to the `RPI-RP2` drive. The tag will reboot and run the new firmware.
