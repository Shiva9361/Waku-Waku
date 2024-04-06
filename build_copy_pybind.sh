#!/bin/bash
cd build
cmake ..
make 
if [ $? -eq 0 ]; then
    rm ../src/gui/backend/*.so
    cp *.so ../src/gui/backend
else
    echo Abort
fi