#!/bin/bash

g++ realsense_gesture_capture.cpp \
  -ggdb `pkg-config --cflags opencv` \
  `pkg-config --libs opencv` \
  -std=c++11 \
  -Iinclude \
  -Llib \
  -lrealsense \
  -lm `pkg-config --cflags --libs glfw3 glu gl` \
  -o bin/capture
