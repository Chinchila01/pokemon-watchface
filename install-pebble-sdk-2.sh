#!/bin/sh
set -ex

pebble sdk install https://github.com/aveao/PebbleArchive/raw/master/SDKCores/sdk-core-4.3.tar.bz2
cd ~/.pebble-sdk
touch NO_TRACKING
cd $TRAVIS_BUILD_DIR