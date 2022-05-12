cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
project(slowpt
  LANGUAGES CXX
)

include_directories(
  base
  object
  appearance
)

set(MAIN_SRC
  main.cpp
)

add_executable(slowpt
  ${MAIN_SRC}
)
