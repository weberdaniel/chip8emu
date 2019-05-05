#include <cstdint>

class Chip8 
{
public:

 // Chip8 has 4k of memory
 std::uint_fast8_t memory[4069]; 

 // Chip8 has 15 8bit genereal purpose CPU registers. The 16th register
 // holds the carry flag.
 std::uint_fast8_t V[16];

 // Chip8 has a program counter
 std::uint_fast8_t pc;
};
