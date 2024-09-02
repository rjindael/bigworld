cmake_minimum_required(VERSION 3.25)

project(bigworld)

find_package(cxxopts CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

include_directories(third-party/irrKlang/include)

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
    link_directories(third-party/irrKlang/lib/win)

    add_custom_command(TARGET bigworld POST_BUILD
        COMMENT "Copying irrKlang binaries for Windows"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/third-party/irrKlang/bin/win/ikpMP3.dll
            ${CMAKE_SOURCE_DIR}/third-party/irrKlang/bin/win/ikpFlac.dll
            ${CMAKE_SOURCE_DIR}/third-party/irrKlang/bin/win/irrKlang.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
else()
    add_executable(bigworld ${SOURCES})
    # what to link to on Linux??

    add_custom_command(TARGET bigworld POST_BUILD
        COMMENT "Copying irrKlang binaries for Linux"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/third-party/irrKlang/bin/linux/ikpMP3.so
            ${CMAKE_SOURCE_DIR}/third-party/irrKlang/bin/linux/ikpFlac.so
            ${CMAKE_SOURCE_DIR}/third-party/irrKlang/bin/linux/irrKlang.so
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()

target_include_directories(bigworld PRIVATE
    src/include
    third-party/irrKlang/include
)

target_link_libraries(bigworld PRIVATE
    cxxopts::cxxopts
    glad::glad
    irrKlang
    SDL2::SDL2 SDL2::SDL2main
)

add_custom_command(TARGET bigworld POST_BUILD
    COMMENT "Copying bigworld assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)