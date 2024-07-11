cmake_minimum_required(VERSION 3.25)

project("GameOfLife" LANGUAGES CXX)

add_executable(life)

# Add CLang-Tidy, if available

find_program(CLANG_TIDY_LOC clang-tidy)

if(CLANG_TIDY_LOC)
  
  message("-- Enabling clang-tidy.\n")
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_LOC})
endif()

# Compiler warning flags go here.
if(NOT CMAKE_CXX_COMPILER_ID  MATCHES MSVC)
  list(APPEND WarningFlags "-Wall" "-Wextra" "-Wpedantic" "-Wno-newline-eof" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wconversion" "-Wshadow" "-Wnon-virtual-dtor")
else()
  list(APPEND WarningFlags "/Wall", "/permissive-")
endif()

target_sources(
    life
    
    PUBLIC

    ${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine.h

    PRIVATE

    ${CMAKE_CURRENT_LIST_DIR}/Source.cpp
)
