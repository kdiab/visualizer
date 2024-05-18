#!/bin/sh
cc ./src/main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Lbuild/ -o ./build/visualizer
