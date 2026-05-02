cmake_minimum_required(VERSION 3.16)
project(SCABS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(SCABS
    scabs_gui.cpp
    scabs_backend.h
)

target_link_libraries(SCABS PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
