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

BOOST_AUTO_TEST_CASE( test_skip_if_equal )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.memory[0x200] = 0x38;
  emu.memory[0x201] = 0x55;
  emu.emulateCycle();
  BOOST_CHECK( emu.pc == 0x204 );
}

BOOST_AUTO_TEST_CASE( test_set_delay_timer )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF8;
  emu.memory[0x201] = 0x15;
  emu.emulateCycle();
  BOOST_CHECK( emu.delay_timer == 0x55 );
}

BOOST_AUTO_TEST_CASE( test_set_sound_timer )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x53;
  emu.V[0x7] = 0x55;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x18;
  emu.emulateCycle();
  BOOST_CHECK( emu.sound_timer == 0x53 );
}

BOOST_AUTO_TEST_CASE( test_add_vx_to_i)
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x53;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x1E;
  emu.emulateCycle();
  BOOST_CHECK( emu.I == 0x54 );
}

BOOST_AUTO_TEST_CASE( skip_next_instruction_if_vx_equals_vy  )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x59;
  emu.memory[0x201] = 0x80;
  emu.emulateCycle();
  BOOST_CHECK( emu.pc == 0x204 );
}

BOOST_AUTO_TEST_CASE( do_not_skip_next_instruction_if_vx_not_equals_vy  )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x8] = 0x51;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x59;
  emu.memory[0x201] = 0x80;
  emu.emulateCycle();
  BOOST_CHECK( emu.pc == 0x202 );
}

BOOST_AUTO_TEST_CASE( add_Vx_to_I )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x8] = 0x01;
  emu.V[0x9] = 0x01;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x1E;
  emu.emulateCycle();
  BOOST_CHECK( emu.I == 0x02 );
}
