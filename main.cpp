#include "chip8.h"
#include <iostream>
#include <memory>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <bitset>

class curses_key_interface : public chip8::KeyInterface {
public:
  curses_key_interface() { };
  std::uint8_t getKey(int to) noexcept {
    timeout(to);
    switch(getch()){
	  case 49: return 1;
	  case 50: return 2;
	  case 51: return 3;
	  case 52: return 4;
	  case 53: return 5;
	  case 54: return 6;
	  case 55: return 7;
	  case 56: return 8;
	  case 57: return 9;
	  case 48: return 0;
	  case 97: return 0xA;
	  case 98: return 0xB;
	  case 99: return 0xC;
	  case 100: return 0xD;
	  case 101: return 0xE;
	  case 102: return 0xF;
  }
  }
};

int main() {
  chip8::emulator emu;
  emu.set_keyinterface(std::make_unique<curses_key_interface>());
  emu.initialize();

  std::ifstream input( "../roms/rom", std::ios::binary );
  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
  std::ofstream output( "output", std::ios::binary );
  
  for( int i = 0; i < buffer.size(); ++i ) {
    emu.memory[i+0x200] = buffer.at(i);
    output << buffer.at(i);
  }
  output.close();

  int height  {32};
  int width   {64};
  int start_y {0};
  int start_x {0};

  initscr();

  WINDOW* main_window    = newwin(height, width, start_y, start_x);
  WINDOW* program_window = newwin(height, width, 0, width);
  WINDOW* memory_window  = newwin(height/2+2, width*2, height, 0 );
  refresh();

  box(program_window, 0, 0);
  box(memory_window,  0, 0);
  wrefresh(main_window);
  refresh();

  while(1) {
    emu.emulateCycle();
    for( int k = 0; k < 32; k++) {
      for( int m = 0; m < 64; m++ ) {
          if( emu.gfx[k][m] )
            mvwprintw(main_window, k, m, "%c", 'x');
	  else
            mvwprintw(main_window, k, m, "%c", ' ');
      }

      for( int l = 0; l < 16; l++ ) {
        mvwprintw(memory_window, l+1, 1, "V[0x%x] = 0x%02x", l, emu.V[l]);
        mvwprintw(memory_window, l+1, 40, "stack[0x%x] = 0x%02x", l, emu.stack[l]);
      }

      mvwprintw(memory_window, 1, 20, "I  = 0x%02x", emu.I);
      mvwprintw(memory_window, 2, 20, "pc = 0x%02x", emu.pc);
      mvwprintw(memory_window, 4, 20, "delay_timer = 0x%02x", emu.delay_timer);
      mvwprintw(memory_window, 5, 20, "sound_timer = 0x%02x", emu.sound_timer);

      for( int l = -28; l < 29; l += 2 ) {
        if( l != 0 )
          mvwprintw(program_window, l/2+1+14, 1, "%03d | 0x%02x%02x", l/2, 
                    emu.memory[emu.pc+l], emu.memory[emu.pc+l+1]);
        else 
          mvwprintw(program_window, l/2+1+14, 1, "%03d | 0x%02x%02x <---", l/2, 
                    emu.memory[emu.pc+l], emu.memory[emu.pc+l+1]);
      }

    }

    wrefresh(main_window);
    wrefresh(program_window);
    wrefresh(memory_window);
    refresh();
    timeout(-1);
    int test = getch();
    mvwprintw(memory_window, 10, 10, "getch = %d", test);
  }

  int a = 0;
  endwin();
}
