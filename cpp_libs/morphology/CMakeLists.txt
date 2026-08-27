cmake_minimum_required(VERSION 3.8)

project(morphology)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(
  main
  src/main.cpp
  src/image.cpp
  src/structuring_element.cpp
  src/morphology.cpp
)

target_include_directories(
  main
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
