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

BOOST_AUTO_TEST_CASE( set_vx_to_nn)
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x3] = 0x00;
  emu.V[0x4] = 0x00;
  emu.V[0x2] = 0x00;
  emu.V[0x8] = 0x01;
  emu.V[0x9] = 0x01;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x63;
  emu.memory[0x201] = 0x1E;
  emu.emulateCycle();
  BOOST_CHECK( emu.V[0x03] == 0x1E );
}

BOOST_AUTO_TEST_CASE( add_nn_to_vx )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x3] = 0x06;
  emu.V[0x4] = 0x00;
  emu.V[0x2] = 0x00;
  emu.V[0x8] = 0x01;
  emu.V[0x9] = 0x01;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x73;
  emu.memory[0x201] = 0x1E;
  emu.emulateCycle();
  BOOST_CHECK( emu.V[0x03] == 0x24 );
}

BOOST_AUTO_TEST_CASE( xortest )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x3] = 0x06;
  emu.V[0x4] = 0x00;

  emu.V[0x2] = 0x11;
  emu.V[0x8] = 0x11;

  emu.V[0x9] = 0xAA;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x73;

  emu.memory[0x202] = 0x82;
  emu.memory[0x203] = 0x83;

  emu.emulateCycle();
  emu.emulateCycle();
  emu.emulateCycle();

  BOOST_CHECK( 0xAA ^ 0x55 == 0xFF );

  BOOST_CHECK( emu.V[0x9] == 0xFF );
  BOOST_CHECK( emu.V[0x2] == 0x00 );
}


BOOST_AUTO_TEST_CASE( and_test )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x3] = 0x06;
  emu.V[0x4] = 0x00;

  emu.V[0x2] = 0x11;
  emu.V[0x8] = 0x11;

  emu.V[0x9] = 0xAA;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x72;

  emu.memory[0x202] = 0x82;
  emu.memory[0x203] = 0x82;

  emu.emulateCycle();
  emu.emulateCycle();
  emu.emulateCycle();

  BOOST_CHECK( emu.V[0x9] == 0x00 );
  BOOST_CHECK( emu.V[0x2] == 0x11 );
}


BOOST_AUTO_TEST_CASE( or_test )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x3] = 0x06;
  emu.V[0x4] = 0x00;

  emu.V[0x2] = 0x11;
  emu.V[0x8] = 0x11;

  emu.V[0x9] = 0xAA;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x71;

  emu.memory[0x202] = 0x82;
  emu.memory[0x203] = 0x81;

  emu.emulateCycle();
  emu.emulateCycle();
  emu.emulateCycle();

  BOOST_CHECK( emu.V[0x9] == 0xFF );
  BOOST_CHECK( emu.V[0x2] == 0x11 );
}

BOOST_AUTO_TEST_CASE( assign_test )
{
  Chip8 emu;
  emu.initialize();
  emu.V[0x3] = 0x06;
  emu.V[0x4] = 0x00;

  emu.V[0x2] = 0x11;
  emu.V[0x8] = 0x12;

  emu.V[0x9] = 0xAA;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x70;

  emu.memory[0x202] = 0x82;
  emu.memory[0x203] = 0x80;

  emu.emulateCycle();
  emu.emulateCycle();
  emu.emulateCycle();

  BOOST_CHECK( emu.V[0x9] == 0x55 );
  BOOST_CHECK( emu.V[0x2] == 0x12 );
}

BOOST_AUTO_TEST_CASE( x_plus_y_test )
{
  Chip8 emu;
  emu.initialize();

  emu.V[0x9] = 0xFF;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x74;

  emu.emulateCycle();

  BOOST_CHECK( emu.V[0x9] == 0x54 );
  BOOST_CHECK( emu.V[16] == 1 );
}

BOOST_AUTO_TEST_CASE( x_minus_y_test )
{
  Chip8 emu;
  emu.initialize();

  emu.V[0x9] = 0x33;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x75;

  emu.emulateCycle();

  BOOST_CHECK( emu.V[0x9] == 0xDE );
  BOOST_CHECK( emu.V[16] == 0 );
}

BOOST_AUTO_TEST_CASE( VxisVyminusVx  )
{
  Chip8 emu;
  emu.initialize();

  emu.V[0x9] = 0x12;
  emu.V[0x7] = 0x10;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x77;

  emu.emulateCycle();

  BOOST_CHECK( emu.V[0x9] == 0xFE );
  BOOST_CHECK( emu.V[16] == 0 );
}

BOOST_AUTO_TEST_CASE( test_9XY0 )
{
  Chip8 emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.V[0x9] = 0x10;
  emu.V[0x7] = 0x10;
  emu.memory[0x200] = 0x99;
  emu.memory[0x201] = 0x70;
  emu.emulateCycle();
  BOOST_CHECK( emu.pc = 0x202 );
  emu.V[0x9] = 0x12;
  emu.V[0x7] = 0x10;
  emu.memory[0x202] = 0x99;
  emu.memory[0x203] = 0x70;
  emu.emulateCycle();
  BOOST_CHECK( emu.pc = 0x206 );
}

BOOST_AUTO_TEST_CASE( test_FX07 )
{
  Chip8 emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x11;
  emu.V[0x9] = 0x00;
  emu.V[0x7] = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x07;
  emu.emulateCycle();
  BOOST_CHECK( emu.V[0x9] = 0x11 );
}
