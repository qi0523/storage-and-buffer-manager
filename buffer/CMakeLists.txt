add_library(libbuffer STATIC buffer.cpp)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(libbuffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(libbuffer PRIVATE libstorage)