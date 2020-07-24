#!/usr/bin/env bash

// armeabi
for arch in  armeabi-v7a arm64-v8a x86 x86_64
do
    bash build_jpeg.sh $arch
done
