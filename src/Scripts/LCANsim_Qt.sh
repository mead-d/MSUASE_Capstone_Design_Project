#!/bin/bash

EXENAME=LCANsim_v1-3-animus

moc ../SimulationWidget.h -o ../moc_SimulationWidget.cpp
echo "Compiling LCANsim"
g++ -g -fPIC -o ../$EXENAME \
    ../main.cpp \
    ../SimulationWidget.cpp ../moc_SimulationWidget.cpp \
    ../Autonode.cpp ../Deployer.cpp ../TargetSelector.cpp \
    -I../../adevs/include \
    -I/usr/include/x86_64-linux-gnu/qt5 \
    -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
    -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
    -L/usr/lib/x86_64-linux-gnu \
    -lQt5Widgets -lQt5Gui -lQt5Core
    #$(pkg-config --cflags --libs Qt5Widgets)
