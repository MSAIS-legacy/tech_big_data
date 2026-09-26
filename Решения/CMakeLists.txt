cmake_minimum_required(VERSION 3.20)
project(tech_big_data_variant_26 LANGUAGES CXX)

if(NOT MSVC)
  message(FATAL_ERROR "These assignments use Microsoft PPL and require MSVC on Windows.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(I)
add_subdirectory(II)
add_subdirectory(III)
add_subdirectory(IV)
add_subdirectory(V)
add_subdirectory(VI)
add_subdirectory(VII)
add_subdirectory(VIII)
