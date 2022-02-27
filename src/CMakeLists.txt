cmake_minimum_required(VERSION 3.2)

project(slowpt
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

set(BASIC_CLASSES
  vec3d.h
  colorRGB.h
  ray.h
)

set(HITTABLE_OBJ
  objectbase.h
  objectbase.cpp
  objectsphere.h
  objectsphere.cpp
)

set(MAIN_SRC
  main.cpp
)

add_executable(slowpt
  ${MAIN_SRC}
  ${BASIC_CLASSES}
  ${HITTABLE_OBJ}
)