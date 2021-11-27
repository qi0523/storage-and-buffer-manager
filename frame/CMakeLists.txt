message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(libbuffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(libstorage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)