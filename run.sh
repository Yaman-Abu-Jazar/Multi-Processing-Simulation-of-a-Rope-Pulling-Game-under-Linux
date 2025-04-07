#!/bin/sh


gcc -g main.c -o main
gcc -g team1.c -o team1
gcc -g team2.c -o team2
gcc -g drawer.c -o drawer -lglut -lGLU -lGL -lm 

./main