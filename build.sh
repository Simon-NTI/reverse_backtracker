#!/bin/sh
rm output
cc -Wall -Wextra "${1:-main.c}" $(pkg-config --libs --cflags raylib) -o "${2:-output}"
./output