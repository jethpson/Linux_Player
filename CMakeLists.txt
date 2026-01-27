cmake_minimum_required(VERSION 3.10)
project(LinuxPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "/usr/lib/qt5")  # Qt5 path
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt5 packages (only Widgets needed now)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Add your source files
file(GLOB SOURCES "src/*.cpp")

# Build executable
add_executable(LinuxPlayer ${SOURCES})

# Link Qt5 Widgets and VLC libraries
find_library(VLC_LIB vlc)
find_library(VLCCORE_LIB vlccore)

target_link_libraries(LinuxPlayer Qt5::Widgets ${VLC_LIB} ${VLCCORE_LIB})