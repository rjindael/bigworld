cmake_minimum_required(VERSION 3.25)

project(bigworld)

find_package(cxxopts CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Application.hpp
    src/InputHandler.cpp
    src/InputHandler.hpp
    src/SoundEngine.cpp
    src/SoundEngine.hpp

    src/Render/RenderEngine.cpp
    src/Render/RenderEngine.hpp
    src/Render/RenderObject.cpp
    src/Render/RenderObject.hpp
    src/Render/Objects/Triangle.cpp
    src/Render/Objects/Triangle.hpp
    src/Render/Objects/Star.cpp
    src/Render/Objects/Star.hpp
)

if(WIN32)
    add_executable(bigworld WIN32 ${SOURCES} assets/bigworld.rc)
else()
    add_executable(bigworld ${SOURCES})
endif()

target_include_directories(bigworld PRIVATE src/include)

target_link_libraries(bigworld PRIVATE
    cxxopts::cxxopts
    glad::glad
    SDL2::SDL2 SDL2::SDL2main
)

add_custom_command(TARGET bigworld POST_BUILD
    COMMENT "Copying bigworld assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)