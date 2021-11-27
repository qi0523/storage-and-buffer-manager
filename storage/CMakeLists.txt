add_library(libstorage STATIC storage.cpp)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(libstorage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)