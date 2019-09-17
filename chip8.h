#include <cstdint>
#include <random>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <chrono>

// Copyright 2019 Daniel Weber

#ifndef CHIP8_H_
#define CHIP8_H_

namespace chip8 {

std::map<std::uint16_t,std::string> opcodes{
  { 0x00EE, "Return from Subroutine" },
  { 0x1000, "Jump to Adr NNN"},  
  { 0x2000, "2NNN: Call subr. at NNN" },  
  { 0x3000, "3XNN: Skip instr. if Vx eq NN" },  
  { 0x4000, "4XNN: Skip instr. if Vx neq NN" },  
};


class KeyInterface {
public:
  KeyInterface() = default;
  KeyInterface(KeyInterface const&) = delete;
  KeyInterface& operator=(KeyInterface const&) = delete;

  virtual std::uint8_t getKey(int to) noexcept = 0; 
};

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

  void set_keyinterface(std::unique_ptr<KeyInterface> arg) {
    keyinterface = std::move(arg);
  };

  constexpr void initialize() noexcept {
    for (auto& x : memory) x = 0;
    for (auto& x : V)      x = 0;
    for (auto& x : stack)  x = 0;
    for (int i = 0; i < 32; i++) {
      for (int k = 0; k < 64; k++) {
        gfx[i][k] = 0x00;
      }
    }
    pc          = 0x200;
    sp          = 0;
    I           = 0;
    delay_timer = 0;
    sound_timer = 0;
    opcode      = 0;

    for (int i = 0; i < 80; i++) {
      memory[i] = fontset[i];
    }
  };

  void emulateCycle(bool test = false) noexcept {
    opcode = memory[pc];
    opcode = opcode << 8;
    opcode = opcode | memory[pc+1];

    //std::cout << opcode << std::endl;

    // DXYN: Draw starting at mem location I, at (Vx, Vy) on
    // screen. Sprites are XORed, if collision with pixel, set
    // VF=1

    if ((opcode & 0xF000) == 0xD000) {
      V[0xF] = 0;
      for (int i = 0; i < (opcode & 0x000F); i++) {
        for (int j = 0; j < 8; j++) {
          if( ((memory[I+i] >> (7-j)) & 1) == 1 ) {
          int t = gfx[(V[(opcode & 0x00F0) >> 4]+i)%32]
                     [(V[(opcode & 0x0F00) >> 8]+j)%64];

          gfx[(V[(opcode & 0x00F0) >> 4]+i)%32]
             [(V[(opcode & 0x0F00) >> 8]+j)%64]
          = (((memory[I+i] >> (7-j)) & 1) != t) ? 1 : 0;

          if ( t != gfx[(V[(opcode & 0x00F0) >> 4]+i)%32]
                       [(V[(opcode & 0x0F00) >> 8]+j)%64]
	       && t==1 ) {
            V[0xF] = 1;
          }
	  }
        }
      }
    }
    
    // EX9E: Skips next instruction if key stored in VX is pressed
    if ((opcode & 0xF0FF) == 0xE09E) {
      if( V[(opcode & 0x0F00) >> 8] == keyinterface.get()->getKey(100) ) {
        pc += 2;
      }
    }
    
    // EXA1: Skips next instruction if key stored in VX is pressed
    if ((opcode & 0xF0FF) == 0xE0A1) {
      if( V[(opcode & 0x0F00) >> 8] != keyinterface.get()->getKey(100) ) {
        pc += 2;
      }
    }

    // FX55: Store V0 to VX in memory, starting at I.
    if ((opcode & 0xF0FF) == 0xF055) {
      for (int i = 0; i <= (opcode & 0x0F00); i++) {
        memory[I+i] = V[i];
      }
    }

    // FX65: Fill V0 to VX with values starting at I.
    if ((opcode & 0xF0FF) == 0xF055) {
      for (int i = 0; i <= (opcode & 0x0F00); i++) {
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
        unsigned int r = time(NULL);
        t = rand_r(&r) % 255;
      }

      V[(opcode & 0x0F00) >> 8] = (t & (opcode & 0x00FF));
    }
    
    // FX29: I = spriteaddr[Vx]
    if ((opcode & 0xF0FF) == 0xF029) {
       I = 5 * V[(opcode & 0x0F00) >> 8];
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
    if ((opcode & 0xF000) == 0x2000) {
      sp++;
      stack[sp] = pc;
      pc = (opcode & 0x0FFF);
      pc -= 2;
    }
    
    // 00EE: The interpreter sets the program
    //       counter to the address at the top
    //       of the stack, then substracts 1 
    //       from the stack pointer
    if ((opcode & 0xFFFF) == 0x00EE) {
      pc = stack[sp];
      sp--;
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
      V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8 ] &
                                  V[(opcode & 0x00F0) >> 4 ]);
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
    
    // FX0A: A key press is awaited, then stored in VX (Blocking)
    if ((opcode & 0xF0FF) == 0xF00A) {
      if( keyinterface.get() ) {
        V[(opcode & 0x0F00) >> 8] = keyinterface.get()->getKey(-1);
      }
    }

    // 8XY5: Vx -= Vy
    if ((opcode & 0xF00F) == 0x8005) {
      // set borrow (inverse logic to carry!!)
      if (V[(opcode & 0x0F00) >> 8 ] > (V[(opcode & 0x00F0) >> 4])) {
        V[0xf] = 1;
      } else {
        V[0xf] = 0;
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
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4 ]
                                  - V[(opcode & 0x0F00) >> 8];
    }

    // 9XY0: skip next if Vx != Vy
    if ((opcode & 0xF00F) == 0x9000) {
      if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
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

    // FX33: Store BCD representation of Vx in memory loc I,I+1,I+2
    if ((opcode & 0xF0FF) == 0xF033) {
      V[I]   = V[(opcode & 0x0F00) >> 8] / 100; 
      V[I+1] = (V[(opcode & 0x0F00) >> 8] - V[I]*100)/10;
      V[I+2] = V[(opcode & 0x0F00) >> 8] - V[I]*100 - V[I+1]*10;
    }

    // 00E0: Clear screen
    if ((opcode & 0xFFFF) == 0x00E0) {
       for (int i = 0; i < 32; i++) {
         for (int j = 0; j < 64; j++) {
           gfx[i][j] = 0x00;
         }
       }
    }
    pc += 2;
    update_timer();
  };

  void update_timer() {
    std::chrono::system_clock::time_point now = 
                                          std::chrono::system_clock::now();
    if(delay_timer != 0) {
      std::chrono::milliseconds delay_delta = 
        std::chrono::duration_cast<std::chrono::milliseconds>(now-last_delay);
      if( (delay_delta > std::chrono::duration<float, std::milli>(1/60)) ) {
        last_delay = now;
        delay_timer--;
      }
    }
    if(sound_timer != 0) {
      std::chrono::milliseconds sound_delta = 
        std::chrono::duration_cast<std::chrono::milliseconds>(now-last_sound);
      if( (sound_delta > std::chrono::duration<float, std::milli>(1/60)) ) {
        last_sound = now;
        sound_timer--;
      }
    }
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
  std::uint8_t gfx[32][64];
  std::unique_ptr<KeyInterface> keyinterface;
  // Save timepoint of last decrease of delay timer
  std::chrono::time_point<std::chrono::system_clock> last_delay;
  // Save timepoint of last decrease of sound timer
  std::chrono::time_point<std::chrono::system_clock> last_sound;
};

}  // namespace chip8

#endif  // CHIP8_H_
