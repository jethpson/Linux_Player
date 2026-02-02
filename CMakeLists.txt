cmake_minimum_required(VERSION 3.16)
project(LinuxPlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------
# Qt6
# --------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# --------------------
# VLC
# --------------------
find_library(VLC_LIB vlc REQUIRED)
find_library(VLCCORE_LIB vlccore REQUIRED)

# --------------------
# Sources
# --------------------
file(GLOB_RECURSE SRC
    src/*.cpp
    src/ui/*.cpp
)

# Only headers with Q_OBJECT need moc
file(GLOB_RECURSE MOC_HEADERS
    src/*.h
    src/ui/*.h
)

# --------------------
# Qt executable (handles moc automatically)
# --------------------
qt_add_executable(LinuxPlayer
    ${SRC}
)

# Make sure moc runs on all headers with Q_OBJECT
qt6_wrap_cpp(MOC_FILES ${MOC_HEADERS})
target_sources(LinuxPlayer PRIVATE ${MOC_FILES})

# --------------------
# Include directories
# --------------------
target_include_directories(LinuxPlayer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# --------------------
# Link libraries
# --------------------
target_link_libraries(LinuxPlayer PRIVATE
    Qt6::Widgets
    ${VLC_LIB}
    ${VLCCORE_LIB}
)
