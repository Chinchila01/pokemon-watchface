#!/bin/sh
set -ex
wget https://developer.rebble.io/s3.amazonaws.com/assets.getpebble.com/pebble-tool/pebble-sdk-4.5-linux64.tar.bz2
tar xvf pebble-sdk-4.5-linux64.tar.bz2
sudo pip install virtualenv

cd pebble-sdk-4.5-linux64
virtualenv --no-site-packages .env
source .env/bin/activate
pip install -r requirements.txt
deactivate