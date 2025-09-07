cmake_minimum_required(VERSION 3.16)

project(DeepRock VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui LinguistTools)

# 添加可执行文件
add_executable(DeepRock
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    gamewindow.cpp
    gamewindow.h
    gameengine.cpp
    gameengine.h
    player.cpp
    player.h
    platform.cpp
    platform.h
    mainwindow.ui

)

# 链接Qt库
target_link_libraries(DeepRock PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

# 包含当前目录
target_include_directories(DeepRock PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 复制资源文件到构建目录








# 将翻译文件添加到资源
target_sources(DeepRock PRIVATE ${QM_FILES})
