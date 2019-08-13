#include "chip8.h"
#include <iostream>
#include <memory>
#include <ncurses.h>

class curses_key_interface : public chip8::KeyInterface {
public:
  curses_key_interface() { };
  std::uint8_t getKey() noexcept {
    return getch();
  }
};

int main() {
  chip8::emulator emu;
  emu.set_keyinterface(std::make_unique<curses_key_interface>());
  emu.initialize();

  int height {32};
  int width {64};
  int start_y {0};
  int start_x {1};

  initscr();

  WINDOW* win = newwin(height, width, start_y, start_x);
  refresh();

  box(win, 0, 0);
  wrefresh(win);

  refresh();

  emu.memory[0x200] = 0xF1;
  emu.memory[0x201] = 0x0A;

  emu.emulateCycle();
  int a = 0;

  wprintw(win, "%i", emu.V[0x1]);
  refresh();

  getch();

  endwin();

}
