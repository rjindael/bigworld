cmake_minimum_required(VERSION 3.25)

project(bigworld)

find_package(cxxopts CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(bigworld WIN32
    src/main.cpp

    # Core application
    src/Application.cpp
    src/Application.hpp
    src/Renderer.cpp
    src/Renderer.hpp
    src/InputHandler.cpp
    src/InputHandler.hpp
)

target_link_libraries(bigworld PRIVATE
    cxxopts::cxxopts
    glad::glad
    SDL2::SDL2 SDL2::SDL2main
)