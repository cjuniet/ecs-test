cmake_minimum_required(VERSION 2.8)
project(ecs-test)

set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/ext/SFML")

set(ENTITYX_BUILD_TESTING FALSE CACHE BOOL "" FORCE)
set(ENTITYX_BUILD_SHARED FALSE CACHE BOOL "" FORCE)
add_subdirectory("ext/entityx")

set(CMAKE_CXX_STANDARD 14)
add_definitions(-DSFML_STATIC)
include_directories("${CMAKE_SOURCE_DIR}/ext/SFML/include")
include_directories("${CMAKE_SOURCE_DIR}/ext/entityx")
include_directories("${CMAKE_SOURCE_DIR}/ext/entt/src")

add_executable(test-entityx main-entityx.cpp)
target_link_libraries(test-entityx entityx sfml-system sfml-window sfml-graphics)

add_executable(test-entt main-entt.cpp)
target_link_libraries(test-entt sfml-system sfml-window sfml-graphics)