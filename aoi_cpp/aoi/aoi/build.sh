#! /bin/sh

cd /data/home/user00/aoi
rm -rf build/
mkdir build
cd build

cmake ..
make 
