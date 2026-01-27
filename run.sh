#!/bin/bash
# rebuild project
cmake -S . -B build
cmake --build build

# launch the executable in Konsole
konsole -e bash -c "export QT_QPA_PLATFORM=xcb; ./build/LinuxPlayer; exit"
