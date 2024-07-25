cmake_minimum_required(VERSION 3.4)
project(waku-waku)
# set (PYTHON_EXECUTABLE "/usr/bin/python3.12")
add_subdirectory(pybind11)
pybind11_add_module(processor src/simulator/processor.cpp src/simulator/assembler.cpp src/simulator/branchPredictor.cpp src/simulator/cache.cpp
src/simulator/hazardDetector.cpp src/simulator/state.cpp src/simulator/core.cpp src/simulator/util.cpp)
