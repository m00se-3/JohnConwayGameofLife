find_program(clang_tidy_FOUND clang-tidy)
find_program(cppcheck_FOUND cppcheck)
find_program(ccache_FOUND ccache)

if(${CMAKE_CXX_COMPILER_ID} MATCHES MSVC)
	include(${CMAKE_CURRENT_LIST_DIR}/MSVCOptions.cmake)
else()
	include(${CMAKE_CURRENT_LIST_DIR}/GNUOptions.cmake)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/UnitTests.cmake)