cmake_minimum_required(VERSION 3.28)
project(3DMazeMod C)

set(CMAKE_C_STANDARD 23)

# To include libraries like the CRT statically
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_library(3DMazeMod SHARED library.c
        Utils/EXEPatchUtils.c
        Utils/EXEPatchUtils.h
        Utils/General.h
        MazeAddrs.h
        Utils/General.c
)
