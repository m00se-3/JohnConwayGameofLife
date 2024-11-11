add_executable(life)

if(clang_tidy_FOUND)
  message("-- Enabling clang-tidy.\n")
  set(CMAKE_CXX_CLANG_TIDY ${clang_tidy_FOUND})
endif()

target_compile_features(life PRIVATE cxx_std_23)
target_compile_options(life PRIVATE ${CompilerFlags})
target_link_options(life PRIVATE ${LinkerFlags})

target_link_libraries(
  life 

  PRIVATE 

  fmt::fmt
)

target_sources(
    life

    PUBLIC 

    FILE_SET HEADERS 

    BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}

    FILES 

    ${CMAKE_CURRENT_LIST_DIR}/GameOfLife.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Camera.hpp

    PRIVATE

    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GameOfLife.cpp
)