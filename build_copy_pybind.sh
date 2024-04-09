#!/bin/bash
if [ -d build ]; then
    cd build;
else
    mkdir build;
    cd build;
fi
cmake ..
make 
if [ $? -eq 0 ]; then
    rm ../src/gui/backend/*.so
    cp *.so ../src/gui/backend
else
    echo Abort
fi