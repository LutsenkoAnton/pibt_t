cmake_minimum_required(VERSION 3.16)
project(pibt_r CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(./pibt_r)
# add_subdirectory(./third_party/googletest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g")
add_executable(pibt mapf.cpp)
target_compile_features(pibt PUBLIC cxx_std_20)
target_link_libraries(pibt lib-mapf-r)

add_executable(rotate_result rotate_result.cpp)
target_compile_features(rotate_result PUBLIC cxx_std_20)
target_link_libraries(rotate_result lib-mapf-r)

add_executable(benchmark benchmark.cpp)
target_compile_features(benchmark PUBLIC cxx_std_20)
target_link_libraries(benchmark lib-mapf-r)

# format
add_custom_target(clang-format
  COMMAND clang-format -i
  ../pibt_r/include/*.hpp
  ../pibt_r/src/*.cpp
  ../graph_r/include/*.hpp
  ../graph_r/src/*.cpp
  # ../tests/*.cpp
  ../rotate_result.cpp
  ../mapf.cpp)
  # ../mapd.cpp)

# test
# set(TEST_MAIN_FUNC ./third_party/googletest/googletest/src/gtest_main.cc)
# set(TEST_ALL_SRC ${TEST_MAIN_FUNC})
# macro(add_test name target)
#   add_executable(${name} ${target} ${TEST_MAIN_FUNC})
#   target_link_libraries(${name} lib-mapf gtest)
#   list(APPEND TEST_ALL_SRC ${target})
# endmacro(add_test)
#
# # basic
# add_test(test_plan ./tests/test_plan.cpp)
# add_test(test_paths ./tests/test_paths.cpp)
# add_test(test_solver ./tests/test_solver.cpp)
# add_test(test_problem ./tests/test_problem.cpp)
# # mapf solvers
# add_test(test_hca ./tests/test_hca.cpp)
# add_test(test_pibt ./tests/test_pibt.cpp)
# add_test(test_push_and_swap ./tests/test_push_and_swap.cpp)
# add_test(test_pibt_plus ./tests/test_pibt_plus.cpp)
# # maps solvers
# add_test(test_pibt_mapd ./tests/test_pibt_mapd.cpp)
# add_test(test_tp ./tests/test_tp.cpp)
#
# add_executable(test ${TEST_ALL_SRC})
# target_link_libraries(test lib-mapf gtest)
