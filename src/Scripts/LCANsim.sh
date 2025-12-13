#!/bin/bash
#CFLAGS = -Wall -g --std= c++13
#INCLUDE = adevs/include

#EXENAME = LCANsim
#CFILES = src/main.cpp src/Autonode.cpp src/Deployer.cpp
echo "Compiling LCANsim"
g++ -I ../../adevs/include -o ../LCANsim_v1-1 ../main.cpp ../Autonode.cpp ../Deployer.cpp
echo "Compilation Complete"\n