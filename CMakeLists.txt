
cmake_minimum_required(VERSION 3.21)

# CodeQuest - Learn, Code & Quiz
# Developer: Munah Sarah Sameer

project(CodeQuest VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(CodeQuest WIN32
    main.cpp
    PlayerWelcome.h
    questions.qrc
)

target_link_libraries(CodeQuest PRIVATE Qt6::Widgets)