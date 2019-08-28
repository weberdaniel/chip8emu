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
    return getch();
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

  int height {32};
  int width {64};
  int start_y {0};
  int start_x {0};

  initscr();

  WINDOW* win = newwin(height, width, start_y, start_x);
  refresh();

  box(win, 0, 0);
  wrefresh(win);

  refresh();

  while(1) {
    emu.emulateCycle();
    for( int k = 0; k < 32; k++) {
      for( int m = 0; m < 64; m++ ) {
          if( emu.gfx[k][m] )
            mvwprintw(win, k, m, "%c", 'x');
      }
    }
    wrefresh(win);
    refresh();
    getch();
  }

  int a = 0;


  endwin();

}
