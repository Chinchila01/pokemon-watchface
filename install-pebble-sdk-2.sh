#!/bin/sh
set -ex

mkdir -p ~/.pebble-sdk
touch ~/.pebble-sdk/NO_TRACKING
pebble sdk install https://github.com/aveao/PebbleArchive/raw/master/SDKCores/sdk-core-4.3.tar.bz2
