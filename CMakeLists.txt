cmake_minimum_required(VERSION 3.25)

project(bigworld)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(bigworld src/main.cpp)
target_link_libraries(bigworld PRIVATE glad::glad glfw)