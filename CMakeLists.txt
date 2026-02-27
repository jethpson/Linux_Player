cmake_minimum_required(VERSION 3.16)
project(LinuxPlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------
# Qt6
# --------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Enable AUTOMOC, AUTOUIC, AUTORCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --------------------
# VLC
# --------------------
find_library(VLC_LIB vlc REQUIRED)
find_library(VLCCORE_LIB vlccore REQUIRED)

# --------------------
# Main App Sources
# --------------------
file(GLOB_RECURSE APP_SOURCES
    src/*.cpp
    src/ui/*.cpp
)

# No need to glob headers for AUTOMOC; just ensure they are included in #include statements in CPPs

# --------------------
# Main Executable
# --------------------
qt_add_executable(LinuxPlayer
    ${APP_SOURCES}
)

set_target_properties(LinuxPlayer PROPERTIES
    ENABLE_EXPORTS ON
)

target_include_directories(LinuxPlayer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

target_link_libraries(LinuxPlayer PRIVATE
    Qt6::Widgets
    ${VLC_LIB}
    ${VLCCORE_LIB}
)

# --------------------
# Plugins
# --------------------
set(PLUGIN_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins)

# Each subfolder in src/plugins = one plugin
file(GLOB PLUGIN_DIRS RELATIVE ${PLUGIN_BASE_DIR} ${PLUGIN_BASE_DIR}/*)

foreach(pluginDir ${PLUGIN_DIRS})
    if(IS_DIRECTORY ${PLUGIN_BASE_DIR}/${pluginDir})

        file(GLOB_RECURSE PLUGIN_SOURCES
            ${PLUGIN_BASE_DIR}/${pluginDir}/*.cpp
            ${PLUGIN_BASE_DIR}/${pluginDir}/*.h
        )

        # Use folder name as plugin target name
        qt_add_library(${pluginDir} MODULE
            ${PLUGIN_SOURCES}
        )

        target_include_directories(${pluginDir} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${PLUGIN_BASE_DIR}/${pluginDir}
        )

        target_link_libraries(${pluginDir} PRIVATE
            Qt6::Widgets
        )

        install(TARGETS ${pluginDir}
            LIBRARY DESTINATION bin/plugins
        )

    endif()
endforeach()