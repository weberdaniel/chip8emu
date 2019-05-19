cmake_minimum_required(VERSION 3.10)
project(chip8emu)

set(BOOST_ROOT $ENV{BOOST_ROOT})
set(XMAKE_CXX_STANDARD 14)
set(SOURCE_FILES chip8.cpp main.cpp chip8.h)
set(TEST_FILES chip8.cpp test.cpp chip8.h)

find_package( Boost 1.70 REQUIRED )

include_directories( ${Boost_INCLUDE_DIR} )

add_executable(chip8emu ${SOURCE_FILES})
target_link_libraries( chip8emu LINK_PUBLIC ${Boost_LIBRARIES} )

add_executable(testchip8emu ${TEST_FILES})
target_link_libraries( testchip8emu LINK_PUBLIC ${Boost_LIBRARIES} )
