# Building

```
git clone https://github.com/raspberrypi/pico-sdk
cd pico-sdk
git submodule init
git submodule update
cd ..
export PICO_SDK_PATH=<full_path_to_pico_sdk>
git clone git@github.com:rgerganov/ggtag
cd ggtag
git submodule init
git submodule update
mkdir build
cd build
cmake .. -DPICO_BOARD=pico
make
```

# Flashing

Press and hold the BOOTSEL button while putting the battery. Copy `build/ggtag.uf2` to the `RPI-RP2` drive. The tag will reboot and run the new firmware.

