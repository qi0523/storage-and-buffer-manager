cmake_minimum_required(VERSION 3.9)

project(sabm)

add_subdirectory(buffer)

add_subdirectory(storage)

add_executable(sabmapp main.cpp)

target_link_libraries(sabmapp libbuffer)