#!/bin/sh
cc ./src/v2.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Lbuild/ -o ./build/visualizer
