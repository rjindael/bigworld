cmake_minimum_required(VERSION 3.25)
project(shambhala C)

option(STRICT_CHECKS "Extra checks for safety during debugging at the expense of performance" ON)

if(STRICT_CHECKS OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(STRICT_CHECKS=1)
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(OpenGL REQUIRED)
find_package(SDL3 CONFIG)

set(SOURCES
    src/main.c
    src/io.c
    src/io.h
    src/shambhala.c
    src/shambhala.h

    # Render engine
    src/render/engine.c
    src/render/engine.h
    src/render/shader.c
    src/render/shader.h
    src/render/mesh.c
    src/render/mesh.h

    # GLAD
    src/glad/src/gl.c
    src/glad/include/glad/gl.h
    src/glad/include/KHR/khrplatform.h
)

add_executable(shambhala ${SOURCES})

target_include_directories(shambhala PRIVATE src src/glad/include)
target_link_libraries(shambhala PRIVATE SDL3::SDL3 OpenGL::GL)