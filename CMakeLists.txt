cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(mtui
  VERSION 0.0.0
  LANGUAGES CXX
)

#-------------------------SET GLOBAL CONSTANTS----------------------------#

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -Werror")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# set(SOURCES
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/progress_bar.cpp
# )
# add_executable (progress ${SOURCES})
#
# target_include_directories(progress
#   PRIVATE
#   ${CMAKE_CURRENT_SOURCE_DIR}/include
# )
