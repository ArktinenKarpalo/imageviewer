cmake_minimum_required(VERSION 3.31)

project(imageviewer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent OpenGLWidgets)
qt_standard_project_setup()

qt_add_executable(imageviewer
    main.cpp
    src/MainWindow.cpp
    src/ImageFile.cpp
    src/ImageManager.cpp
    src/ImageView.cpp
    src/ImageCache.cpp
    src/Utils.cpp
)

target_link_libraries(imageviewer PRIVATE Qt6::Core Qt6::Widgets Qt6::Concurrent Qt6::OpenGLWidgets)


