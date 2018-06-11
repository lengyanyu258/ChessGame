project(Gomoku_Terminator)

# the executable program file name
set(PFX_NAME pbrain-)
set(EXE_NAME Gomoku_Terminator)

# check the system platform architecture(x86 or x64).
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(EXE_ARCH 64)
endif()

# does this system provide the random function?
include(CheckFunctionExists)

check_function_exists(random HAVE_RANDOM)

# check the system platform
if(WIN32)
	set(ON_WIN32 1)
	set(ON_UNIX 0)
elseif(UNIX)
	set(ON_WIN32 0)
	set(ON_UNIX 1)
	# required by the <thread> header file.
	link_libraries(pthread)
endif()

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
	${PROJECT_SOURCE_DIR}/config.h.in
	${PROJECT_BINARY_DIR}/config.h
)

# add the binary tree to the search path for include files
# so that we will find config.h
include_directories(${PROJECT_BINARY_DIR})

# add optimal algorithm program
add_executable(${PFX_NAME}${EXE_NAME}${EXE_ARCH}-normal
        protocol/pisqpipe.cpp algorithm/ai.h algorithm/normal.cpp
        brain.cpp Chessboard.cpp)

# add uct algorithm program
add_executable(${PFX_NAME}${EXE_NAME}${EXE_ARCH}-uct
		protocol/pisqpipe.cpp algorithm/ai.h algorithm/uct.cpp
        brain.cpp Chessboard.cpp)

# add the example project.
add_subdirectory(protocol EXCLUDE_FROM_ALL)
