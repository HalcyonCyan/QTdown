cmake_minimum_required(VERSION 3.16)

# 设置项目名称和版本
project(DeepRock VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动处理UI、资源和MOC文件
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

# 查找所需的Qt模块
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network  # 添加网络模块
)

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

    # 新增文件
    account.cpp
    account.h
    network.cpp
    network.h
    leaderboard.cpp
    leaderboard.h
)

# 链接Qt库
target_link_libraries(DeepRock PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network  # 链接网络模块
)

# 设置包含目录
target_include_directories(DeepRock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 在Windows上设置子系统（如果是GUI应用程序）
if(WIN32)
    set_target_properties(DeepRock PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
