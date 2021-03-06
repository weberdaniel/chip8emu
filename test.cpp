// Copyright 2019 Daniel Weber All rights reserved

#define BOOST_TEST_MODULE chip8test
#include <iostream>
#include "./chip8.h"
#include <boost/test/included/unit_test.hpp>
BOOST_AUTO_TEST_CASE(test_init) {
  chip8::emulator emu;
  emu.initialize();
  int i = 0;
  for (i = 0; i < 80; i++)
    BOOST_CHECK(emu.memory[i] == chip8::fontset[i]);

  for (i = 80; i < sizeof(emu.memory); i++)
    BOOST_CHECK(emu.memory[i] == 0);

  for (auto& x : emu.V )
    BOOST_CHECK(x == 0);

  for (auto& x : emu.stack )
    BOOST_CHECK(x == 0);

  BOOST_CHECK(emu.pc == 0x200);
  BOOST_CHECK(emu.sp == 0);
  BOOST_CHECK(emu.I  == 0);
  BOOST_CHECK(emu.delay_timer == 0);
  BOOST_CHECK(emu.sound_timer == 0);
}

BOOST_AUTO_TEST_CASE(test_set_index) {
  chip8::emulator emu;
  emu.initialize();
  emu.memory[0x200] = 0xA0;
  emu.memory[0x201] = 0x9C;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc == 0x202);
  BOOST_CHECK(emu.I == 0x009C);
}

BOOST_AUTO_TEST_CASE(test_goto) {
  chip8::emulator emu;
  emu.initialize();
  emu.memory[0x200] = 0x14;
  emu.memory[0x201] = 0x11;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc == 0x411);
}

BOOST_AUTO_TEST_CASE(test_skip_if_equal) {
  chip8::emulator emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.memory[0x200] = 0x38;
  emu.memory[0x201] = 0x55;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc == 0x204);
}

BOOST_AUTO_TEST_CASE(test_set_delay_timer) {
  chip8::emulator emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF8;
  emu.memory[0x201] = 0x15;
  emu.emulateCycle();
  BOOST_CHECK(emu.delay_timer <= 0x55);
}

BOOST_AUTO_TEST_CASE(test_set_sound_timer ) {
  chip8::emulator emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x53;
  emu.V[0x7] = 0x55;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x18;
  emu.emulateCycle();
  BOOST_CHECK(emu.sound_timer <= 0x53);
}

BOOST_AUTO_TEST_CASE(test_add_vx_to_i) {
  chip8::emulator emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x53;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x1E;
  emu.emulateCycle();
  BOOST_CHECK(emu.I == 0x54);
}

BOOST_AUTO_TEST_CASE(skip_next_instruction_if_vx_equals_vy) {
  chip8::emulator emu;
  emu.initialize();
  emu.V[0x8] = 0x55;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x59;
  emu.memory[0x201] = 0x80;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc == 0x204);
}

BOOST_AUTO_TEST_CASE(do_not_skip_next_instruction_if_vx_not_equals_vy) {
  chip8::emulator emu;
  emu.initialize();
  emu.V[0x8] = 0x51;
  emu.V[0x9] = 0x55;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x59;
  emu.memory[0x201] = 0x80;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc == 0x202);
}

BOOST_AUTO_TEST_CASE(add_Vx_to_I) {
  chip8::emulator emu;
  emu.initialize();
  emu.V[0x8] = 0x01;
  emu.V[0x9] = 0x01;
  emu.V[0x7] = 0x55;
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x1E;
  emu.emulateCycle();
  BOOST_CHECK(emu.I == 0x02);
}

BOOST_AUTO_TEST_CASE(set_vx_to_nn) {
  chip8::emulator emu;
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
  BOOST_CHECK(emu.V[0x03] == 0x1E);
}

BOOST_AUTO_TEST_CASE(add_nn_to_vx ) {
  chip8::emulator emu;
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
  BOOST_CHECK(emu.V[0x03] == 0x24);
}

BOOST_AUTO_TEST_CASE(xortest) {
  chip8::emulator emu;
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

  BOOST_CHECK(0xAA ^ 0x55 == 0xFF);

  BOOST_CHECK(emu.V[0x9] == 0xFF);
  BOOST_CHECK(emu.V[0x2] == 0x00);
}


BOOST_AUTO_TEST_CASE(and_test) {
  chip8::emulator emu;
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

  BOOST_CHECK(emu.V[0x9] == 0x00);
  BOOST_CHECK(emu.V[0x2] == 0x11);
}


BOOST_AUTO_TEST_CASE(or_test) {
  chip8::emulator emu;
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

  BOOST_CHECK(emu.V[0x9] == 0xFF);
  BOOST_CHECK(emu.V[0x2] == 0x11);
}

BOOST_AUTO_TEST_CASE(assign_test) {
  chip8::emulator emu;
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

  BOOST_CHECK(emu.V[0x9] == 0x55);
  BOOST_CHECK(emu.V[0x2] == 0x12);
}

BOOST_AUTO_TEST_CASE(x_plus_y_test) {
  chip8::emulator emu;
  emu.initialize();

  emu.V[0x9] = 0xFF;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x74;

  emu.emulateCycle();

  BOOST_CHECK(emu.V[0x9] == 0x54);
  BOOST_CHECK(emu.V[16] == 1);
}

BOOST_AUTO_TEST_CASE(x_minus_y_test) {
  chip8::emulator emu;
  emu.initialize();

  emu.V[0x9] = 0x33;
  emu.V[0x7] = 0x55;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x75;

  emu.emulateCycle();

  BOOST_CHECK(emu.V[0x9] == 0xDE);
  BOOST_CHECK(emu.V[0xf] == 0);
}

BOOST_AUTO_TEST_CASE(VxisVyminusVx) {
  chip8::emulator emu;
  emu.initialize();

  emu.V[0x9] = 0x12;
  emu.V[0x7] = 0x10;

  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0x77;

  emu.emulateCycle();

  BOOST_CHECK(emu.V[0x9] == 0xFE);
  BOOST_CHECK(emu.V[16] == 0);
}

BOOST_AUTO_TEST_CASE(test_9XY0) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x00;
  emu.V[0x9] = 0x10;
  emu.V[0x7] = 0x10;
  emu.memory[0x200] = 0x99;
  emu.memory[0x201] = 0x70;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc = 0x202);
  emu.V[0x9] = 0x12;
  emu.V[0x7] = 0x10;
  emu.memory[0x202] = 0x99;
  emu.memory[0x203] = 0x70;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc = 0x206);
}

BOOST_AUTO_TEST_CASE(test_FX07) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x11;
  emu.V[0x9] = 0x00;
  emu.V[0x7] = 0x00;
  emu.memory[0x200] = 0xF9;
  emu.memory[0x201] = 0x07;
  emu.emulateCycle();
  BOOST_CHECK(emu.V[0x9] = 0x11);
}

BOOST_AUTO_TEST_CASE(test_BNNN) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x11;
  emu.V[0] = 0x01;
  emu.V[0x9] = 0x00;
  emu.V[0x7] = 0x00;
  emu.memory[0x200] = 0xB9;
  emu.memory[0x201] = 0x98;
  emu.emulateCycle();
  BOOST_CHECK(emu.pc = 0x999);
}

BOOST_AUTO_TEST_CASE(test_CXNN) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x11;
  emu.V[0] = 0x01;
  emu.V[0x9] = 0xFF;
  emu.V[0x7] = 0xFF;
  emu.memory[0x200] = 0xC7;
  emu.memory[0x201] = 0xAA;
  emu.emulateCycle(true);
  BOOST_CHECK(emu.V[0x7] == 0x00);
  emu.memory[0x202] = 0xC7;
  emu.memory[0x203] = 0x55;
  emu.emulateCycle(true);
  BOOST_CHECK(emu.V[0x7] == 0x55);
}

BOOST_AUTO_TEST_CASE(test_8xye) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x11;
  emu.V[0] = 0x01;
  emu.V[0xF] = 0x0;
  emu.V[0x9] = 0xFF;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0xFE;
  emu.emulateCycle();
  BOOST_CHECK(emu.V[0x9] == 0xFE);
  BOOST_CHECK(emu.V[0xF] == 1);
}

BOOST_AUTO_TEST_CASE(test_8xy6) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x11;
  emu.V[0] = 0x01;
  emu.V[0xF] = 0x0;
  emu.V[0x9] = 0xFF;
  emu.memory[0x200] = 0x89;
  emu.memory[0x201] = 0xF6;
  emu.emulateCycle();
  BOOST_CHECK(emu.V[0x9] == 0x7F);
  BOOST_CHECK(emu.V[0xF] == 1);
}

BOOST_AUTO_TEST_CASE(clear_screen) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x01;
  emu.delay_timer = 0x11;
  emu.V[0] = 0x01;
  emu.V[0xF] = 0x0;
  emu.V[0x9] = 0xFF;
  emu.memory[0x200] = 0x00;
  emu.memory[0x201] = 0xE0;

  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 64; j++) {
      emu.gfx[i][j] = 0xFF;
    }
  }
  emu.emulateCycle();

  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 64; j++) {
      BOOST_CHECK(emu.gfx[i][j] == 0x00);
    }
  }
}

BOOST_AUTO_TEST_CASE(draw_zero_test) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x0;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0x0;
  emu.V[0xF] = 0x0;
  emu.memory[0x200] = 0xD1;
  emu.memory[0x201] = 0x15;

  emu.emulateCycle();

  BOOST_CHECK(emu.gfx[0][0] == 1);
  BOOST_CHECK(emu.gfx[1][0] == 1);
  BOOST_CHECK(emu.gfx[2][0] == 1);
  BOOST_CHECK(emu.gfx[3][0] == 1);
  BOOST_CHECK(emu.gfx[4][0] == 1);
  BOOST_CHECK(emu.gfx[5][0] == 0);

  BOOST_CHECK(emu.gfx[0][1] == 1);
  BOOST_CHECK(emu.gfx[1][1] == 0);
  BOOST_CHECK(emu.gfx[2][1] == 0);
  BOOST_CHECK(emu.gfx[3][1] == 0);
  BOOST_CHECK(emu.gfx[4][1] == 1);
  BOOST_CHECK(emu.gfx[5][1] == 0);

  BOOST_CHECK(emu.gfx[0][2] == 1);
  BOOST_CHECK(emu.gfx[1][2] == 0);
  BOOST_CHECK(emu.gfx[2][2] == 0);
  BOOST_CHECK(emu.gfx[3][2] == 0);
  BOOST_CHECK(emu.gfx[4][2] == 1);
  BOOST_CHECK(emu.gfx[5][2] == 0);

  BOOST_CHECK(emu.gfx[0][3] == 1);
  BOOST_CHECK(emu.gfx[1][3] == 1);
  BOOST_CHECK(emu.gfx[2][3] == 1);
  BOOST_CHECK(emu.gfx[3][3] == 1);
  BOOST_CHECK(emu.gfx[4][3] == 1);
  BOOST_CHECK(emu.gfx[5][3] == 0);

  BOOST_CHECK(emu.V[0xF] == 0);
}

BOOST_AUTO_TEST_CASE(draw_wrap) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x0;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0x1;
  emu.V[0x2] = 62;
  emu.V[0xF] = 0x0;
  emu.memory[0x200] = 0xD2;
  emu.memory[0x201] = 0x15;

  emu.emulateCycle();

  BOOST_CHECK(emu.gfx[1][62] == 1);
  BOOST_CHECK(emu.gfx[2][62] == 1);
  BOOST_CHECK(emu.gfx[3][62] == 1);
  BOOST_CHECK(emu.gfx[4][62] == 1);
  BOOST_CHECK(emu.gfx[5][62] == 1);
  BOOST_CHECK(emu.gfx[6][62] == 0);

  BOOST_CHECK(emu.gfx[1][63] == 1);
  BOOST_CHECK(emu.gfx[2][63] == 0);
  BOOST_CHECK(emu.gfx[3][63] == 0);
  BOOST_CHECK(emu.gfx[4][63] == 0);
  BOOST_CHECK(emu.gfx[5][63] == 1);
  BOOST_CHECK(emu.gfx[6][63] == 0);

  BOOST_CHECK(emu.gfx[1][0] == 1);
  BOOST_CHECK(emu.gfx[2][0] == 0);
  BOOST_CHECK(emu.gfx[3][0] == 0);
  BOOST_CHECK(emu.gfx[4][0] == 0);
  BOOST_CHECK(emu.gfx[5][0] == 1);
  BOOST_CHECK(emu.gfx[6][0] == 0);

  BOOST_CHECK(emu.gfx[1][1] == 1);
  BOOST_CHECK(emu.gfx[2][1] == 1);
  BOOST_CHECK(emu.gfx[3][1] == 1);
  BOOST_CHECK(emu.gfx[4][1] == 1);
  BOOST_CHECK(emu.gfx[5][1] == 1);
  BOOST_CHECK(emu.gfx[6][1] == 0);

  BOOST_CHECK(emu.V[0xF] == 0);
}

BOOST_AUTO_TEST_CASE(delete_pixel_test) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0x0;
  emu.V[0xF] = 0x0;
  emu.memory[0x200] = 0xD1;
  emu.memory[0x201] = 0x15;
  emu.gfx[1][1] = 1;
  emu.gfx[2][2] = 1;
  emu.gfx[1][3] = 1;
  emu.gfx[3][3] = 1;

  emu.emulateCycle();

  BOOST_CHECK(emu.gfx[0][0] == 0);
  BOOST_CHECK(emu.gfx[1][0] == 0);
  BOOST_CHECK(emu.gfx[2][0] == 0);
  BOOST_CHECK(emu.gfx[3][0] == 0);
  BOOST_CHECK(emu.gfx[4][0] == 0);
  BOOST_CHECK(emu.gfx[5][0] == 0);

  BOOST_CHECK(emu.gfx[0][1] == 0);
  //this will be deleted due to collision
  BOOST_CHECK(emu.gfx[1][1] == 0);
  BOOST_CHECK(emu.gfx[2][1] == 0);
  BOOST_CHECK(emu.gfx[3][1] == 0);
  BOOST_CHECK(emu.gfx[4][1] == 1);
  BOOST_CHECK(emu.gfx[5][1] == 0);

  BOOST_CHECK(emu.gfx[0][2] == 1);
  BOOST_CHECK(emu.gfx[1][2] == 1);
  //will also be deleted.
  BOOST_CHECK(emu.gfx[2][2] == 0);
  BOOST_CHECK(emu.gfx[3][2] == 1);
  BOOST_CHECK(emu.gfx[4][2] == 1);
  BOOST_CHECK(emu.gfx[5][2] == 0);

  BOOST_CHECK(emu.gfx[0][3] == 0);
  //will not be changed
  BOOST_CHECK(emu.gfx[1][3] == 1);
  BOOST_CHECK(emu.gfx[2][3] == 0);
  //3,3 is set to zero, initially 1, so xor will result in 1
  BOOST_CHECK(emu.gfx[3][3] == 1);
  BOOST_CHECK(emu.gfx[4][3] == 1);
  BOOST_CHECK(emu.gfx[5][3] == 0);

  //collision will be true
  BOOST_CHECK(emu.V[0xF] == 1);
}

BOOST_AUTO_TEST_CASE(draw_one_test) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0x0;
  emu.V[0xF] = 0x0;
  emu.memory[0x200] = 0xD1;
  emu.memory[0x201] = 0x15;

  emu.emulateCycle();

  BOOST_CHECK(emu.gfx[0][0] == 0);
  BOOST_CHECK(emu.gfx[1][0] == 0);
  BOOST_CHECK(emu.gfx[2][0] == 0);
  BOOST_CHECK(emu.gfx[3][0] == 0);
  BOOST_CHECK(emu.gfx[4][0] == 0);
  BOOST_CHECK(emu.gfx[5][0] == 0);

  BOOST_CHECK(emu.gfx[0][1] == 0);
  BOOST_CHECK(emu.gfx[1][1] == 1);
  BOOST_CHECK(emu.gfx[2][1] == 0);
  BOOST_CHECK(emu.gfx[3][1] == 0);
  BOOST_CHECK(emu.gfx[4][1] == 1);
  BOOST_CHECK(emu.gfx[5][1] == 0);

  BOOST_CHECK(emu.gfx[0][2] == 1);
  BOOST_CHECK(emu.gfx[1][2] == 1);
  BOOST_CHECK(emu.gfx[2][2] == 1);
  BOOST_CHECK(emu.gfx[3][2] == 1);
  BOOST_CHECK(emu.gfx[4][2] == 1);
  BOOST_CHECK(emu.gfx[5][2] == 0);

  BOOST_CHECK(emu.gfx[0][3] == 0);
  BOOST_CHECK(emu.gfx[1][3] == 0);
  BOOST_CHECK(emu.gfx[2][3] == 0);
  BOOST_CHECK(emu.gfx[3][3] == 0);
  BOOST_CHECK(emu.gfx[4][3] == 1);
  BOOST_CHECK(emu.gfx[5][3] == 0);

  BOOST_CHECK(emu.V[0xF] == 0);
}

BOOST_AUTO_TEST_CASE(fx29_test) {
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0x0;
  emu.V[0x2] = 0xA;
  emu.V[0xF] = 0xA;
  emu.memory[0x200] = 0xF2;
  emu.memory[0x201] = 0x29;
  emu.emulateCycle();
  BOOST_CHECK(emu.I == (0xA*5));

  emu.memory[0x202] = 0xD1;
  emu.memory[0x203] = 0x15;
  emu.emulateCycle();

  BOOST_CHECK(emu.gfx[0][0] == 1);
  BOOST_CHECK(emu.gfx[1][0] == 1);
  BOOST_CHECK(emu.gfx[2][0] == 1);
  BOOST_CHECK(emu.gfx[3][0] == 1);
  BOOST_CHECK(emu.gfx[4][0] == 1);

  BOOST_CHECK(emu.gfx[0][1] == 1);
  BOOST_CHECK(emu.gfx[1][1] == 0);
  BOOST_CHECK(emu.gfx[2][1] == 1);
  BOOST_CHECK(emu.gfx[3][1] == 0);
  BOOST_CHECK(emu.gfx[4][1] == 0);

  BOOST_CHECK(emu.gfx[0][2] == 1);
  BOOST_CHECK(emu.gfx[1][2] == 0);
  BOOST_CHECK(emu.gfx[2][2] == 1);
  BOOST_CHECK(emu.gfx[3][2] == 0);
  BOOST_CHECK(emu.gfx[4][2] == 0);

  BOOST_CHECK(emu.gfx[0][3] == 1);
  BOOST_CHECK(emu.gfx[1][3] == 1);
  BOOST_CHECK(emu.gfx[2][3] == 1);
  BOOST_CHECK(emu.gfx[3][3] == 1);
  BOOST_CHECK(emu.gfx[4][3] == 1);

  BOOST_CHECK(emu.V[0xF] == 0x0);
}

BOOST_AUTO_TEST_CASE(regdump_test) {
  chip8::emulator emu;
  emu.initialize();
  emu.delay_timer = 0x11;
  emu.V[0x0] = 0x03;
  emu.V[0x1] = 0x1A;
  emu.I = 0x300;
  emu.memory[0x200] = 0xF1;
  emu.memory[0x201] = 0x55;
  emu.emulateCycle();
  BOOST_CHECK(emu.memory[0x300] == 0x03);
  BOOST_CHECK(emu.memory[0x301] == 0x1A);
  BOOST_CHECK(emu.memory[0x302] == 0x00);
}

BOOST_AUTO_TEST_CASE(regload_test) {
  chip8::emulator emu;
  emu.initialize();
  emu.delay_timer = 0x11;
  emu.memory[0x300] = 0x03;
  emu.memory[0x301] = 0x1A;
  emu.memory[0x302] = 0x00;
  emu.V[0x0] = 0x00;
  emu.V[0x1] = 0x00;
  emu.I = 0x300;
  emu.memory[0x200] = 0xF1;
  emu.memory[0x201] = 0x65;
  emu.emulateCycle();
  BOOST_CHECK(emu.V[0x0] == 0x03);
  BOOST_CHECK(emu.V[0x1] == 0x1A);
  BOOST_CHECK(emu.V[0x2] == 0x00);
}

BOOST_AUTO_TEST_CASE(key_test) {
  class keytest_interface : public chip8::KeyInterface {
  public:
    keytest_interface() { };
    std::uint8_t getKey(int to) noexcept {
      return 0xC;
    }
  };
  chip8::emulator emu;
  emu.initialize();
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0x0;
  emu.memory[0x200] = 0xF1;
  emu.memory[0x201] = 0x0A;
  emu.set_keyinterface(std::make_unique<keytest_interface>());
  emu.emulateCycle();

  BOOST_CHECK(emu.V[0x1] == 0xC);

}

BOOST_AUTO_TEST_CASE(keypressedinvx_test) {
  class keytest_interface : public chip8::KeyInterface {
  public:
    keytest_interface() { };
    std::uint8_t getKey(int to) noexcept {
      return 0xC;
    }
  };
  chip8::emulator emu;
  emu.initialize();
  emu.set_keyinterface(std::make_unique<keytest_interface>());
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0xC;
  emu.V[0x2] = 0x5;
  // skip next instruction if key is pressed stored in V[1]
  emu.memory[0x200] = 0xE1;
  emu.memory[0x201] = 0x9E;
  // next instruction: add V[2] to V[1]
  emu.memory[0x202] = 0x81;
  emu.memory[0x203] = 0x24;
  emu.emulateCycle();
  emu.emulateCycle();
  // v1 should still be the same value 
  BOOST_CHECK(emu.V[0x1] == 0xC);
}

BOOST_AUTO_TEST_CASE(keypressedinvx_2_test) {
  class keytest_interface : public chip8::KeyInterface {
  public:
    keytest_interface() { };
    std::uint8_t getKey(int to) noexcept {
      return 0x0;
    }
  };
  chip8::emulator emu;
  emu.initialize();
  emu.set_keyinterface(std::make_unique<keytest_interface>());
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0xC;
  emu.V[0x2] = 0x5;
  // skip next instruction if key is pressed stored in V[1]
  emu.memory[0x200] = 0xE1;
  emu.memory[0x201] = 0x9E;
  // next instruction: add V[2] to V[1]
  emu.memory[0x202] = 0x81;
  emu.memory[0x203] = 0x24;
  emu.emulateCycle();
  emu.emulateCycle();
  // v1 should still be the same value 
  BOOST_CHECK(emu.V[0x1] == 0xC+0x5);
  BOOST_CHECK(emu.V[0xF] == 0);
}

BOOST_AUTO_TEST_CASE(exa1_test) {
  class keytest_interface : public chip8::KeyInterface {
  public:
    keytest_interface() { };
    std::uint8_t getKey(int to) noexcept {
      return 0x0;
    }
  };
  chip8::emulator emu;
  emu.initialize();
  emu.set_keyinterface(std::make_unique<keytest_interface>());
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0xC;
  emu.V[0x2] = 0x5;
  // skip next instruction if key is pressed stored in V[1]
  emu.memory[0x200] = 0xE1;
  emu.memory[0x201] = 0xA1;
  // next instruction: add V[2] to V[1]
  emu.memory[0x202] = 0x81;
  emu.memory[0x203] = 0x24;
  emu.emulateCycle();
  emu.emulateCycle();
  // v1 should still be the same value 
  BOOST_CHECK(emu.V[0x1] == 0xC);
  BOOST_CHECK(emu.V[0xF] == 0);
}

BOOST_AUTO_TEST_CASE(test_return_from_subroutine) {
  class keytest_interface : public chip8::KeyInterface {
  public:
    keytest_interface() { };
    std::uint8_t getKey(int to) noexcept {
      return 0x0;
    }
  };
  chip8::emulator emu;
  emu.initialize();
  emu.set_keyinterface(std::make_unique<keytest_interface>());
  emu.I = 0x5;
  emu.delay_timer = 0x11;
  emu.V[0x1] = 0xC;
  emu.V[0x2] = 0x5;
  emu.V[0x9] = 0x0;
  emu.V[0xA] = 0x0;

  // store a subroutine in memory at loc 0x300-0x303
  // sets V9 to 0x12 and returns from subroutine
  emu.memory[0x300] = 0x69;
  emu.memory[0x301] = 0x12;
  emu.memory[0x302] = 0x00;
  emu.memory[0x303] = 0xEE;

  // Call subroutine at 0x300
  emu.memory[0x200] = 0x23;
  emu.memory[0x201] = 0x00;
  
  // set VA to 0x45
  emu.memory[0x202] = 0x6A;
  emu.memory[0x203] = 0x45;

  //call subroutine
  emu.emulateCycle();

  BOOST_CHECK(emu.sp == 0x1);
  BOOST_CHECK(emu.pc == 0x300);

  // assign
  emu.emulateCycle();
  BOOST_CHECK(emu.V[0x9] == 0x12);

  //return from subroutine 
  emu.emulateCycle();
  
  BOOST_CHECK(emu.sp == 0x0);
  BOOST_CHECK(emu.pc == 0x202);

  // assign
  emu.emulateCycle();

  BOOST_CHECK(emu.V[0x9] == 0x12);
  BOOST_CHECK(emu.V[0xA] == 0x45);
}

BOOST_AUTO_TEST_CASE(test_store_bcd) {
  class keytest_interface : public chip8::KeyInterface {
  public:
    keytest_interface() { };
    std::uint8_t getKey(int to) noexcept {
      return 0x0;
    }
  };

  chip8::emulator emu;
  emu.initialize();
  emu.set_keyinterface(std::make_unique<keytest_interface>());
  emu.I = 0x300;
  emu.delay_timer = 0x11;
  emu.V[0x4] = 123;
  emu.V[0x5] = 0x0;
  emu.V[0x6] = 0x0;
  emu.V[0x7] = 0x0;

  emu.memory[0x200] = 0xF4;
  emu.memory[0x201] = 0x33;

  emu.emulateCycle();

  BOOST_CHECK(emu.V[0x4] == 123);
  BOOST_CHECK(emu.V[0x5] == 0);
  BOOST_CHECK(emu.V[0x6] == 0);
  BOOST_CHECK(emu.V[0x7] == 0);
  BOOST_CHECK(emu.memory[0x300] == 1);
  BOOST_CHECK(emu.memory[0x301] == 2);
  BOOST_CHECK(emu.memory[0x302] == 3);
}
