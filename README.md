# Chip8emu


CHIP-8 is an interpreted programming language developed by Joseph Weisbecker. It was used on the 
CoSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. 


| Opcode | Type    | Pseudocode   | Action
|--------|---------|--------------|---------------------------------------------
| 0NNN   | Call    |              |
| 00E0   | Display |              | Clears the screen
| 00EE   | Flow    | return       | Returns from a subroutine
| 1NNN   | Flow    | goto NNN     | Jump to Address NNN
| 2NNN   | Flow    | \*(0xNNN)()  | Calls subroutine at NNN
| 3XNN   | Cond    | if(Vx==NN)() | Skip the next instruction if Vx eq NN
| 4XNN   | Cond    | if(Vx!=NN)   | Skips next instruction if VX doesn't equal

