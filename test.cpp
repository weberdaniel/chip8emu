#include "chip8.h"
#include <iostream>

#define BOOST_TEST_MODULE chip8test
#include <boost/test/included/unit_test.hpp>

struct cout_redirect {

  cout_redirect( std::streambuf * new_buffer)
    : old( std::cout.rdbuf(new_buffer) )
  { }

  ~cout_redirect() {
    std::cout.rdbuf( old );
  }

 private:
  std::streambuf * old;
};

BOOST_AUTO_TEST_CASE( test_init )
{
  Chip8 emu;
  emu.initialize();

  for( auto& x : emu.memory )
    BOOST_CHECK( x == 0);

  for( auto& x : emu.V )
    BOOST_CHECK( x == 0);
  
  for( auto& x : emu.stack )
    BOOST_CHECK( x == 0);

  for( auto& x : emu.gfx)
    BOOST_CHECK( x == 0);

  BOOST_CHECK( emu.pc == 0x200 );
  BOOST_CHECK( emu.sp == 0);
  BOOST_CHECK( emu.I  == 0);
  BOOST_CHECK( emu.delay_timer == 0);
  BOOST_CHECK( emu.sound_timer == 0);
}

BOOST_AUTO_TEST_CASE( test_set_index )
{

  Chip8 emu;
  emu.initialize();
  emu.memory[0x200] = 0xA0;
  emu.memory[0x201] = 0x9C;
  emu.emulateCycle();
  BOOST_CHECK( emu.pc == 0x202 );
  BOOST_CHECK( emu.I == 0x009C );
}

BOOST_AUTO_TEST_CASE( test_goto )
{
  Chip8 emu;
  emu.initialize();
  emu.memory[0x200] = 0x14;
  emu.memory[0x201] = 0x11;
  emu.emulateCycle();
  BOOST_CHECK( emu.pc == 0x411 );
}
