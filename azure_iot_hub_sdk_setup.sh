#!/bin/bash

echo "Setting up Environment for IoTHub SDK...."

sudo apt-get update
sudo apt-get install -y libssl-dev git cmake build-essential curl libcurl4-openssl-dev uuid-dev

cd ~
mkdir AIQuizIoTHub
cd AIQuizIoTHub

git clone -b LTS_07_2021_Ref01 https://github.com/Azure/azure-iot-sdk-c.git
cd azure-iot-sdk-c
git submodule update --init

cd build_all/linux
./build.sh --no-make

cd ../../cmake/iotsdk_linux

make
sudo make install

cd ../../..

# git clone https://github.com/WiringPi/WiringPi
# cd ./WiringPi
# ./build
# cd ..

git clone https://github.com/kgabis/parson.git
cd ./parson
mv parson.c parson.h ..
cd ..
rm -rf parson

cmake . && make

echo "Environment set up finished successfully"
