cmake_minimum_required(VERSION 3.4)
project(waku-waku)
add_subdirectory(pybind11)
pybind11_add_module(processor src/simulator/processor.cpp)