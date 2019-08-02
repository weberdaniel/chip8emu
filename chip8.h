#include <cstdint>
#include <iostream>

unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

struct Chip8 
{

  constexpr void initialize() noexcept 
  {
    for( auto& x : memory ) x = 0;
    for( auto& x : V )      x = 0;
    for( auto& x : stack )  x = 0;
    for( auto& x: gfx)      x = 0;
    pc          = 0x200;
    sp          = 0;
    I           = 0;
    delay_timer = 0;
    sound_timer = 0;
    opcode      = 0;
  };

  constexpr void emulateCycle() noexcept 
  {
    //opcode = (( std::uint16_t) memory[pc] )<< 8 | memory[pc+1];
    opcode = memory[pc];
    opcode = opcode << 8;
    opcode = opcode | memory[pc+1];

    // ANNN: MEM: I = NNN, Set I to the Address of NNN.
    if( (opcode & 0xF000 ) == 0xA000 ) {
      I = opcode & 0x0FFF;
    }

    // 1NNN: goto NNN
    if( (opcode & 0xF000) == 0x1000 ) {
      pc = opcode & 0x0FFF;
      pc -= 2;
    }
    
    // 3XNN: Skip next instruction if V[X] == NN
    if( (opcode & 0xF000) == 0x3000 ) {
      if( (opcode & 0x00FF) == V[(opcode & 0x0F00) >> 8])
      pc += 2;
    }
    
    // 4XNN: Skip next instruction if V[X] does not equal NN
    if( (opcode & 0xF000) == 0x4000 ) {
      if( (opcode & 0x00FF) != V[(opcode & 0x0F00) >> 8])
      pc += 2;
    }
    
    // 6XNN: Set Vx to NN
    if( (opcode & 0xF000) == 0x6000 ) {
      V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    }
    
    // 7XNN: Add NN to Vx 
    if( (opcode & 0xF000) == 0x7000 ) {
      V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    }
    
    // 5XY0: Skip next instruction if Vx equals Vy
    if( (opcode & 0xF00F) == 0x5000 ) {
      if( V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4] ) {
        pc += 2;
      }
    }

    // FX15 set delay timer to Vx
    if( (opcode & 0xF0FF ) == 0xF015 ) {
       delay_timer = V[(opcode & 0x0F00)  >> 8 ];
    }	      
    
    // FX15 set sound timer to Vx
    if( (opcode & 0xF0FF ) == 0xF018 ) {
       sound_timer = V[(opcode & 0x0F00)  >> 8 ];
    }	      

    // FX1E: adds Vx to I
    if( (opcode & 0xF0FF ) == 0xF01E ) {
       I += V[(opcode & 0x0F00) >> 8];
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
