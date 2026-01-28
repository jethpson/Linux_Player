#!/bin/bash
# rebuild project
cmake -S . -B build || { echo "CMake configuration failed"; exit 1; }
cmake --build build || { echo "Build failed"; exit 1; }

# Launch the executable in Konsole and close when done
konsole -e bash -c "export QT_QPA_PLATFORM=xcb; ./build/LinuxPlayer"
