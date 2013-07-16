project(Kaption)

cmake_minimum_required(VERSION 2.6.2)

set(KDE_MIN_VERSION "4.4.0")
find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

# Add the compiler flags
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src)
add_subdirectory(po)

macro_display_feature_log()
