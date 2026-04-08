cmake_minimum_required(VERSION 3.14)
project(helloredis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

find_path(
  HIREDIS_INCLUDE_DIR
  NAMES hiredis/hiredis.h
)
find_library(HIREDIS_LIBRARY NAMES hiredis)

find_path(
  REDIS_PLUS_PLUS_INCLUDE_DIR
  NAMES redis++.h
  PATH_SUFFIXES sw/redis++
)
find_library(REDIS_PLUS_PLUS_LIBRARY NAMES redis++)

if(NOT HIREDIS_INCLUDE_DIR OR NOT HIREDIS_LIBRARY)
  message(
    FATAL_ERROR
      "hiredis not found. Install hiredis or set CMAKE_PREFIX_PATH to its prefix."
  )
endif()
if(NOT REDIS_PLUS_PLUS_INCLUDE_DIR OR NOT REDIS_PLUS_PLUS_LIBRARY)
  message(
    FATAL_ERROR
      "redis-plus-plus not found. Install redis++ or set CMAKE_PREFIX_PATH to its prefix."
  )
endif()

add_executable(helloredis main.cpp)

target_include_directories(
  helloredis
  PRIVATE ${HIREDIS_INCLUDE_DIR} ${REDIS_PLUS_PLUS_INCLUDE_DIR}
)

target_link_libraries(
  helloredis
  PRIVATE ${REDIS_PLUS_PLUS_LIBRARY} ${HIREDIS_LIBRARY} Threads::Threads
)
