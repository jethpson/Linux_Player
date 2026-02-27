#!/bin/bash

# Rebuild project
cmake -S . -B build || { echo "CMake configuration failed"; exit 1; }
cmake --build build || { echo "Build failed"; exit 1; }

# Activate the virtual environment
source /home/jakei/Projects/Linux_Player/venv/bin/activate

# Launch the executable in Konsole with the environment activated
konsole -e bash -c "export QT_QPA_PLATFORM=xcb; ./build/LinuxPlayer"