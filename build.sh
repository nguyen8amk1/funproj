#!/bin/bash
gcc ./calculator.c -Ivendors/raylib/include -Lvendors/raylib/lib -Wl,-Bstatic -lraylib -Wl,-Bdynamic -lm -ldl -lpthread -lGL -lX11 -lXrandr -lXi -lXinerama -lXcursor && ./a.out
