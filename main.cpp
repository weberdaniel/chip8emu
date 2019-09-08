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
	  case '1': return 1;
	  case '2': return 2;
	  case '3': return 3;
	  case '4': return 4;
	  case '5': return 5;
	  case '6': return 6;
	  case '7': return 7;
	  case '8': return 8;
	  case '9': return 9;
	  case '0': return 0;
	  case 'A': return 0xA;
	  case 'B': return 0xB;
	  case 'C': return 0xC;
	  case 'D': return 0xD;
	  case 'E': return 0xE;
	  case 'F': return 0xF;
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
    getch();
  }

  int a = 0;
  endwin();
}
