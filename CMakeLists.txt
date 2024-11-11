cmake_minimum_required(VERSION 3.25)

project("GameOfLife" LANGUAGES CXX)

include("CPM.cmake")
include("cmake/Config.cmake")

if(ccache_FOUND)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${ccache_FOUND})
  set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_LIST_DIR}/.cache/CPM)
endif()

cpmaddpackage("gh:fmtlib/fmt#11.0.2")

add_subdirectory("life")