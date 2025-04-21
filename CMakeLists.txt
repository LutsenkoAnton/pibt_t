cmake_minimum_required(VERSION 3.16)
project(pibt_t CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(./pibt_t)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g")
add_executable(pibt mapf.cpp)
target_compile_features(pibt PUBLIC cxx_std_20)
target_link_libraries(pibt lib-mapf-t)

add_executable(rotate_result rotate_result.cpp)
target_compile_features(rotate_result PUBLIC cxx_std_20)
target_link_libraries(rotate_result lib-mapf-t)

add_executable(benchmark benchmark.cpp)
target_compile_features(benchmark PUBLIC cxx_std_20)
target_link_libraries(benchmark lib-mapf-t)
