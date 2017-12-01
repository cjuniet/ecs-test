cmake_minimum_required(VERSION 2.8)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)

include_directories("$ENV{HOME}/local/include")
link_directories("$ENV{HOME}/local/lib")

set(SOURCES
  main.cpp
)

add_executable(ecs-test ${SOURCES})
target_link_libraries(ecs-test entityx sfml-system sfml-window sfml-graphics)
