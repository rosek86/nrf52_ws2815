# nRF52840 WS2815 driver

There is only MacOS bootstrap / compilation support. Linux can be easily acheived by updating links in the bootstrap.sh file.

## Bootstrap

Script downloads nRF SDK and gcc compiler.

```bash
./bootstrap.sh
```

## Build

```bash
mkdir Debug && cd Debug
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ..
ninja
```

## Flash via JLinkExe

```bash
cd Debug # or Release
JLinkExe -Device NRF52 -Speed 4000 -If SWD -Autoconnect 1
r
exec SetRTTAddr 0x0000000020001a08
loadfile src/nrf52_ws2815.hex
g
```

## View RTT logs

Connect via JLinkExe first, then using other terminal:

```bash
JLinkRTTClient
```
