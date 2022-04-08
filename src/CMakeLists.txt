cmake_minimum_required(VERSION 3.2)

project(slowpt
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

set(BASIC_FILES
  ./base/vec3d.h
  ./base/colorRGB.h
  ./base/ray.h
  ./base/rtutilities.h
  camera.h
  material.h
)

set(HITTABLE_OBJ
  ./object/objectbase.h
  ./object/objectlist.h
  ./object/objectsphere.h
)

set(MAIN_SRC
  main.cpp
)

add_executable(slowpt
  ${MAIN_SRC}
  ${BASIC_FILES}
  ${HITTABLE_OBJ}
)
