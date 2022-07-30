cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
project(slowpt
  LANGUAGES CXX
)

include_directories(
  src/base
  src/object
  src/appearance
  src/thirdparty
)

set(MAIN_SRC
  src/main.cpp
)

add_executable(slowpt
  ${MAIN_SRC}
)

add_executable(test
  test.cpp
)
