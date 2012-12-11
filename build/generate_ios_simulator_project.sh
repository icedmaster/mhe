#!/bin/sh

cmake . -DCMAKE_TOOLCHAIN_FILE=cmake_modules/iOS.cmake -DPLATFORM_OS=SIMULATOR -GXcode