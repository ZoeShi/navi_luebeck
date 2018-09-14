cmake_minimum_required(VERSION 3.12)
project(navi_luebeck)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})

add_executable(navi_luebeck src/JsonLoader.cpp src/main.cpp src/dijkstra.cpp src/dijkstra.h src/JsonLoader.h src/main.h src/DataTypes.h)

target_link_libraries(navi_luebeck ${GTKMM_LIBRARIES})

