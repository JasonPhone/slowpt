cmake_minimum_required(VERSION 3.2)

project(slowpt
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

set(BASIC_FILES
  vec3d.h
  colorRGB.h
  ray.h
  rtutilities.h
)

set(HITTABLE_OBJ
  objectbase.h
  objectsphere.h
)

set(MAIN_SRC
  main.cpp
)

add_executable(slowpt
  ${MAIN_SRC}
  ${BASIC_FILES}
  ${HITTABLE_OBJ}
)