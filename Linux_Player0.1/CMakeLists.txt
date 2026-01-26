cmake_minimum_required(VERSION 3.10)
project(LinuxPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "/usr/lib/qt5")  # Qt5 path
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt5 packages
find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

# Add your source files
file(GLOB SOURCES "src/*.cpp")

# Build executable
add_executable(LinuxPlayer ${SOURCES})

# Link Qt libraries to the correct target name
target_link_libraries(LinuxPlayer Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets)