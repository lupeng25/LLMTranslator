cmake_minimum_required(VERSION 3.16)
project(LLMTranslator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 指定使用 D:\Qt\Qt5.12.9 的 Qt 版本，而不是 conda 版本
set(CMAKE_PREFIX_PATH "D:/Qt/Qt5.12.9/5.12.9/msvc2017_64" ${CMAKE_PREFIX_PATH})

find_package(Qt5 COMPONENTS Widgets Network Xml REQUIRED)

add_executable(LLMTranslator WIN32
    main.cpp
    MainWindow.cpp
    MainWindow.h
    TranslatorEngine.cpp
    TranslatorEngine.h
    LLMTranslator.rc
)

target_link_libraries(LLMTranslator PRIVATE
    Qt5::Widgets
    Qt5::Network
    Qt5::Xml
)
