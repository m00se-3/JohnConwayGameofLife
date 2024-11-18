# Enable compiler flags for certain build configurations
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    list(
        APPEND CompilerFlags 
        "-g"
        "-D_GLIBCXX_ASSERTIONS"
    )
elseif(${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
    list(
        APPEND CompilerFlags 
        "-g"
        "-02"
        "-U_FORTIFY_SOURCE"
        "-D_FORTIFY_SOURCE=3"
        "-D_GLIBCXX_ASSERTIONS"
    )
else()
    list(
        APPEND CompilerFlags 
        "-02"
        "-U_FORTIFY_SOURCE"
        "-D_FORTIFY_SOURCE=2"
    )
endif()

# Enable special flags when using GCC
if(${CMAKE_CXX_COMPILIER_ID} MATCHES GNU)
    list(
        APPEND CompilerFlags 

        "-fhardened"
        "-Wbidi-chars=any"
        "-Wtrampolines"
        "-Whardened"
    )
endif()

#Exclude unsupported flags on Windows
if(NOT ${CMAKE_HOST_WIN32})
    list(
        APPEND CompilerFlags

        "-mbranch-protection=standard"
    )

    list(
        APPEND LinkerFlags

        "-fPIC"
        "-fPIE"
    )
endif()


# Enable flags for all builds
list(
	APPEND CompilerFlags 

	"-Wall" 
	"-Wextra" 
    "-Wformat"
    "-Wformat=2"
    "-Wimplicit-fallthrough"
    "-Werror=format-security"
    "-Werror=implicit"
    "-Werror=incompatible-pointer-types"
    "-Werror=int-conversion"
	"-Wpedantic" 
	"-Wno-newline-eof" 
	"-Wno-c++98-compat" 
	"-Wno-c++98-compat-pedantic"
	"-Wconversion"
    "-Wsign-conversion"

    "-fstrict-flex-arrays=3"
    "-fcf-protection=full"
    "-fno-strict-overflow"
    "-fno-strict-aliasing"
    "-ftrivial-auto-var-init=zero"
    "-fexceptions"
)

list(
    APPEND LinkerOptions

    "-Wl,-z,nodlopen"
    "-Wl,-z,noexecstack"
    "-Wl,-z,relro"
    "-Wl,--as-needed"
    "-Wl,--no-copy-dt-needed-entries"
    "-pie"
    "-shared"
)
