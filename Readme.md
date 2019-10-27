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
