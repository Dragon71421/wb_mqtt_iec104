#!/bin/bash

# clean previous build files
rm -rf ./build

# create bin directory and open it
mkdir ./build
cp ./src/test_config.json ./build/
cd ./build

# build all
cmake ../
make