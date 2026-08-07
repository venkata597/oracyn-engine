cmake_minimum_required(VERSION 3.15)
project(Oracyn VERSION 0.01 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
add_executable(engine ${SOURCES})

target_link_libraries(engine PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    glm::glm
    OpenGL::GL
    GLEW::GLEW
)
