cmake_minimum_required(VERSION 3.10)
project(chip8emu)
set(XMAKE_CXX_STANDARD 14)

set(SOURCE_FILES chip8.cpp main.cpp chip8.h)

add_executable(chip8emu ${SOURCE_FILES})
