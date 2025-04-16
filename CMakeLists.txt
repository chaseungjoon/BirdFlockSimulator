cmake_minimum_required(VERSION 3.30)
project(BirdFlockSimulator)

set(CMAKE_CXX_STANDARD 20)

set(SFML_DIR "/opt/homebrew/opt/sfml/lib/cmake/SFML")
find_package(SFML REQUIRED COMPONENTS Graphics Window System)

add_executable(BirdFlockSimulator main.cpp
        BirdShape.hpp
        BirdShape.cpp)
target_link_libraries(BirdFlockSimulator
        SFML::Graphics
        SFML::Window
        SFML::System
)