set(EXE_PRFX pbrain-)
set(EXE_NAME random)

project(${EXE_PRFX}${EXE_NAME})

include(CheckFunctionExists)
check_function_exists(random HAVE_RANDOM)

if(WIN32)
	set(ON_WIN32 1)
	set(ON_UNIX 0)
elseif(UNIX)
	set(ON_WIN32 0)
	set(ON_UNIX 1)
endif()

configure_file(
	"${PROJECT_SOURCE_DIR}/skel_cpp_config.h.in"
	"${PROJECT_BINARY_DIR}/skel_cpp_config.h"
)

# add the binary tree to the search path for include files
# so that we will find skel_cpp_config.h
include_directories(${PROJECT_BINARY_DIR})

if(ON_WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(EXE_ARCH 64)
	endif()
	add_executable(${PROJECT_NAME}${EXE_ARCH} pisqpipe.cpp example.cpp)
endif()