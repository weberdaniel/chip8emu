#include "chip8.h"
#include <iostream>

int main() 
{
  chip8::emulator emu;
  emu.initialize();

  std::cout << "hello world";
}
