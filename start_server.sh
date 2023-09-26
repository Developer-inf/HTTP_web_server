#!/bin/bash

for file_cpp in *.cpp
do
    g++ -std=c++20 -c $file_cpp
done

g++ -o main *.o
rm *.o