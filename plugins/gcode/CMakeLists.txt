#/*******************************************************************************
#* Author    :  Damir Bakiev                                                    *
#* Version   :  na                                                              *
#* Date      :  01 February 2020                                                *
#* Website   :  na                                                              *
#* Copyright :  Damir Bakiev 2016-2022                                          *
#* License:                                                                     *
#* Use, modification & distribution is subject to Boost Software License Ver 1. *
#* http://www.boost.org/LICENSE_1_0.txt                                         *
#********************************************************************************/
cmake_minimum_required(VERSION 3.20)


project(gcode)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

#include_directories()

file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB UIS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_library(${PROJECT_NAME} STATIC
    ${HEADERS}
    ${SOURCES}
    ${UIS}
)

link_directories(
    ${CMAKE_SOURCE_DIR}/libs${PATHS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    #    common
    #    filetree
    #    graphicsview
    #    shape
    #    tooldatabase
    file_plugins
    clipper
    gi
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

#add_subdirectory(thermal2)

add_subdirectory(profile)
add_subdirectory(pocketoffset)

add_subdirectory(drill)
add_subdirectory(hatching)
add_subdirectory(pocketraster)
add_subdirectory(thermal)
add_subdirectory(voroni)


