cmake_minimum_required(VERSION 3.20)
project(SFML_Project)
set(CMAKE_CXX_STANDARD 17)

# بگو دقیقاً کجاست sfml@2
set(SFML_DIR "/opt/homebrew/Cellar/sfml@2/2.6.2/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(SFML_Project
        main.cpp
        SudokuGenerator.cpp
)

target_link_libraries(SFML_Project PRIVATE
        sfml-graphics sfml-window sfml-system
)

# برای پیدا شدن .dylib موقع اجرا
set_target_properties(SFML_Project PROPERTIES
        BUILD_RPATH "/opt/homebrew/Cellar/sfml@2/2.6.2/lib"
        INSTALL_RPATH "/opt/homebrew/Cellar/sfml@2/2.6.2/lib"
)
