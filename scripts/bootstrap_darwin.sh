#!/bin/bash

SDK_ZIP=nRF5_SDK_16.0.0_98a08e2.zip
SDK_URL=http://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v16.x.x/$SDK_ZIP
GCC_ZIP=gcc-arm-none-eabi-7-2018-q2-update-mac.tar.bz2
GCC_URL=https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/7-2018q2/$GCC_ZIP

mkdir -p support
cd support

if [[ ! -d sdk ]]; then
  echo "Downloading SDK..."
  curl $SDK_URL --output $SDK_ZIP

  echo "Unzipping SDK..."
  mkdir -p sdk
  unzip $SDK_ZIP -d sdk/
  rm -f $SDK_ZIP
fi

if [[ ! -d gcc-arm-none-eabi-7-2018-q2-update ]]; then
  echo "Downloading GCC..."
  curl $GCC_URL --output $GCC_ZIP

  echo "Unzipping GCC..."
  tar -xjf $GCC_ZIP
  rm -f $GCC_ZIP
fi

echo "OK"