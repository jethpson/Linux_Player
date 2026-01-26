#!/bin/bash
# rebuild project
cmake -S . -B build
cmake --build build

# launch the executable in Konsole
konsole -e bash -c "./build/LinuxPlayer; exit"