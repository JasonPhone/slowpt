cmake_minimum_required(VERSION 3.2)
project(main
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

set(TEST_SRC
  main.cpp
)

add_executable(main ${TEST_SRC})
