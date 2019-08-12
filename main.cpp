#include "chip8.h"
#include <iostream>
#include <ncurses.h>

int main() 
{
  chip8::emulator emu;
  emu.initialize();

  initscr();

  refresh();

  getch();

  endwin();

}
