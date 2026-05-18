cmake_minimum_required(VERSION 3.15)
project(Oracyn VERSION 0.01 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
add_executable(engine ${SOURCES})    

target_link_libraries(engine PRIVATE SDL2::SDL2)