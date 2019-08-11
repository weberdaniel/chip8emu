#include <cstdint>
#include <random>
#include <iostream>
#include <cstdlib>

// Copyright 2019 Daniel Weber

#ifndef CHIP8_H_
#define CHIP8_H_

namespace chip8 {

constexpr unsigned char fontset[80] = {
0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
0x20, 0x60, 0x20, 0x20, 0x70,  // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

struct emulator {
  constexpr void initialize() noexcept {
    for ( auto& x : memory ) x = 0;
    for ( auto& x : V )      x = 0;
    for ( auto& x : stack )  x = 0;
    for ( auto& x : gfx)     x = 0;
    pc          = 0x200;
    sp          = 0;
    I           = 0;
    delay_timer = 0;
    sound_timer = 0;
    opcode      = 0;

    for( int i = 0; i < 80; i++) {
      memory[i] = fontset[i];
    }
  };

  constexpr void emulateCycle(bool test = false) noexcept {
    opcode = memory[pc];
    opcode = opcode << 8;
    opcode = opcode | memory[pc+1];
    
    // DXYN: Draw starting at mem location I, at (Vx, Vy) on
    // screen. Sprites are XORed, if collision with pixel, set 
    // VF=1 

    /*if ((opcode & 0xF000) == 0xD000) {
      V[0xF] = 0;
      for(int i = 0; i < ((opcode & 0x000F)*8); i++ ) {
         if(gfx[V[(opcode & 0x0F00)]*64+V[(opcode & 0x00F0)]+i] 
	    xor memory[I+i] != gfx[V[(opcode & 0x0F00)]*64+

	    V[(opcode & 0x00F0)]+i]) {
           V[0xF] = 1;
         }
        gfx[V[(opcode & 0x0F00)]*64+V[(opcode & 0x00F0)]+i] = 
        gfx[V[(opcode & 0x0F00)]*64+V[(opcode & 0x00F0)]+i] xor memory[I+i];

      }
    }*/

    // FX55: Store V0 to VX in memory, starting at I.
    if ((opcode & 0xF0FF) == 0xF055) {
      for( int i = 0; i <= (opcode & 0x0F00); i++ ) {
        memory[I+i] = V[i];
      }
    }
    
    // FX65: Fill V0 to VX with values starting at I.
    if ((opcode & 0xF0FF) == 0xF055) {
      for( int i = 0; i <= (opcode & 0x0F00); i++ ) {
        V[i] = memory[I+i];
      }
    }

    // 8XYE: Store MSB of VX in VF and shift VX to left by 1
    if ((opcode & 0xF00F) == 0x800E) {
      V[0xF] = (V[(opcode & 0x0F00) >> 8] & 128) >> 7;
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
    }
    
    // 8XY6: Store LSB of VX in VF and shift VX to right by 1
    if ((opcode & 0xF00F) == 0x8006) {
      V[0xF] = V[(opcode & 0x0F00) >> 8] & 1;
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
    }

    // ANNN: MEM: I = NNN, Set I to the Address of NNN.
    if ((opcode & 0xF000) == 0xA000) {
      I = opcode & 0x0FFF;
    }
    
    // BNNN: PC = V0+NNN
    if ((opcode & 0xF000) == 0xB000) {
      pc = V[0] + (opcode & 0x0FFF);
    }
    
    // CXNN: Vx = rand() & NN
    if ((opcode & 0xF000) == 0xC000) {
      std::uint8_t t = 0;

      if (test == true) { 
        t = 0x55; 
      } else {
	t = rand() % 255;
      } 

      V[(opcode & 0x0F00) >> 8] = (t & (opcode & 0x00FF));
    }
 
    // FX07: Vx = getdelay()
    if ((opcode & 0xF0FF) == 0xF007) {
      V[((opcode & 0x0F00) >> 8)] = delay_timer;
    }

    // 1NNN: goto NNN
    if ((opcode & 0xF000) == 0x1000) {
      pc = opcode & 0x0FFF;
      pc -= 2;
    }

    // 2NNN: call subroutine at NNN: interpreter
    //       increments stack pointer then puts
    //       current pc on the top of the stack.
    //       the pc is then set to nnn.
    if ((opcode & 0xF000) == 0x200) {
      sp++;
      stack[sp] = pc;
      pc = (opcode & 0x0FFF);
    }

    // 3XNN: Skip next instruction if V[X] == NN
    if ((opcode & 0xF000) == 0x3000) {
      if ((opcode & 0x00FF) == V[(opcode & 0x0F00) >> 8])
      pc += 2;
    }

    // 4XNN: Skip next instruction if V[X] does not equal NN
    if ((opcode & 0xF000) == 0x4000) {
      if ((opcode & 0x00FF) != V[(opcode & 0x0F00) >> 8])
      pc += 2;
    }
    
    // 5XY0: Skip next instruction if Vx equals Vy
    if ((opcode & 0xF00F) == 0x5000) {
      if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
        pc += 2;
      }
    }

    // 6XNN: Set Vx to NN
    if ((opcode & 0xF000) == 0x6000) {
      V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    }

    // 7XNN: Add NN to Vx
    if ((opcode & 0xF000) == 0x7000) {
      V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    }

    // 8XY0: Vx = Vy
    if ((opcode & 0xF00F) == 0x8000) {
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4 ];
    }

    // 8XY1: Vx = Vx & Vy
    if ((opcode & 0xF00F) == 0x8001) {
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8 ] |
                                  V[(opcode & 0x00F0) >> 4 ];
    }

    // 8XY2: Vx = Vx & Vy
    if ((opcode & 0xF00F) == 0x8002) {
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8 ] &
                                  V[(opcode & 0x00F0) >> 4 ];
    }

    // 8XY3: Vx = Vx ^ Vy
    if ((opcode & 0xF00F) == 0x8003) {
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8 ] ^
                                  V[(opcode & 0x00F0) >> 4 ];
    }

    // 8XY4: Vx += Vy
    if ((opcode & 0xF00F) == 0x8004) {
      // set carry
      if (V[(opcode & 0x00F0) >> 4] > (0xFF-V[(opcode & 0x0F00) >> 8])) {
        V[16] = 1;
      } else {
        V[16] = 0;
      }
      V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4 ];
    }

    // 8XY5: Vx += Vy
    if ((opcode & 0xF00F) == 0x8005) {
      // set borrow (inverse logic to carry!!)
      if (V[(opcode & 0x00F0) >> 4 ] > (V[(opcode & 0x0F00) >> 8])) {
        V[16] = 0;
      } else {
        V[16] = 1;
      }
      V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4 ];
    }
    
    // 8XY7: Vx = Vy-Vx
    if ((opcode & 0xF00F) == 0x8007) {
      // set borrow (inverse logic to carry!!)
      if (V[(opcode & 0x0FF0) >> 8 ] > (V[(opcode & 0x00F0) >> 4])) {
        V[16] = 0;
      } else {
        V[16] = 1;
      }
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4 ] - V[(opcode & 0x0F00) >> 8];
    }
    
    // 9XY0: skip next if Vx != Vy
    if ((opcode & 0xF00F) == 0x9000) {
      if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4 ]) 
        pc += 2;
    }

    // FX15 set delay timer to Vx
    if ((opcode & 0xF0FF) == 0xF015) {
       delay_timer = V[(opcode & 0x0F00)  >> 8 ];
    }

    // FX18 set sound timer to Vx
    if ((opcode & 0xF0FF) == 0xF018) {
       sound_timer = V[(opcode & 0x0F00)  >> 8 ];
    }

    // FX1E: adds Vx to I
    if ((opcode & 0xF0FF) == 0xF01E) {
       I += V[(opcode & 0x0F00) >> 8];
    }
    
    // 00E0: Clear screen
    if ((opcode & 0xFFFF) == 0x00E0) {
       for( auto &i : gfx ) {
         i = 0;
       }
    }

    pc += 2;
  };

  // opcode
  std::uint16_t opcode;
  // Chip8 has 4k of memory
  std::uint8_t memory[4069];
  // Chip8 has 15 8bit genereal purpose CPU registers. The 16th register
  // holds the carry flag.
  std::uint8_t V[16];
  // Chip8 has a program counter
  std::uint16_t pc;
  // Chip8 has a stack pointer
  std::uint8_t sp;
  std::uint16_t stack[16];
  // Chip8 has a index register
  std::uint16_t I;
  // Timer registers
  std::uint8_t delay_timer;
  std::uint8_t sound_timer;
  // Chip8 has a grafic screen of black and white pixel
  std::uint8_t gfx[64 * 32];
};

}

#endif  // CHIP8_H_
