#include <cstdint>

struct Chip8 
{

  constexpr void initialize() noexcept 
  {
    for( auto& x : memory ) x = 0;
    for( auto& x : V ) x = 0;
    pc = 0x200;
    sp = 0;
    for( auto& x : stack ) x = 0;
    I = 0;
    delay_timer = 0;
    sound_timer= 0;
    for( auto& x: gfx) x = 0;
  };

  constexpr void emulateCycle() noexcept 
  {
  
  };

  // Chip8 has 4k of memory
  std::uint_fast8_t memory[4069]; 
  // Chip8 has 15 8bit genereal purpose CPU registers. The 16th register
  // holds the carry flag.
  std::uint_fast8_t V[16];
  // Chip8 has a program counter
  std::uint_fast16_t pc;
  // Chip8 has a stack pointer
  std::uint_fast8_t sp;
  std::uint_fast16_t stack[16];
  // Chip8 has a index register
  std::uint_fast16_t I;
  // Timer registers
  std::uint_fast8_t delay_timer;
  std::uint_fast8_t sound_timer;
  // Chip8 has a grafic screen of black and white pixel
  std::uint_fast8_t gfx[64 * 32];

};
