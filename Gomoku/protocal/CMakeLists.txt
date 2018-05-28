project(example)
set(EXE_NAME random)

add_executable(${PFX_NAME}${EXE_NAME}${EXE_ARCH} pisqpipe.cpp example.cpp)
