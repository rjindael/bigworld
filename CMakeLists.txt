cmake_minimum_required(VERSION 3.25)

project(bigworld)

find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(bigworld WIN32 src/main.cpp)
target_link_libraries(bigworld PRIVATE glad::glad SDL2::SDL2 SDL2::SDL2main)