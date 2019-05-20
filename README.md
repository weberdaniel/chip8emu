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
| 5XY0   | Cond    | if(Vx==Vy)   | Skips the next instruction if Vx equals Vy
| 6XNN   | Const   | Vx = NN      | Sets Vx to NN
| 7XNN   | Const   | Vx += NN     | Adds NN to VX. (Carry flag not changed)
| 8XY0   | Assign  | Vx=Vy        | Sets VX to the value of VY
| 8XY1   | BitOp   | Vx=Vx|Vy     | Sets VX to VX or VY
| 8XY2   | BitOp   | Vx=Vx&Vy     | Sets VX to VX and VY
| 8XY3   | BitOp   | Vx=Vx^Vy     | Sets VX to VX xor VY
| 8XY4   | Math    | Vx += Vy     | Adds VY to Vx. VF is set to 1 if there is a carry, else to 0.
| 8XY5   | Math    | Vx -= Vy     | VY is substracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
| 8XY6   | BitOp   | Vx >>= 1     | Store the least significant bit of VX in VF and shift VX to right by 1
| 8XY7   | Math    | Vx=Vy-Vx     | Set VX to VY minus VX. VF is set to 0 when there's a borrow
| 8XYE   | BitOp   | Vx<<=1       | Store MSB of VX in VF and shift VX to left by 1
| 9XY0   | Cond    | if(Vx!=Vy)   | Skip next instruction if VX doesn't equal VY
| ANNN   | MEM     | 1 = NNN      | Sets I to the address of NNN
| BNNN   | Flow    | PC=V0+NNN    | Jump to address NNN plus V0
| CXNN   | Rand    | Vx=rand()&NN | Sets Vx to the result of a bitwise and op. on a random number and NN
| DXYN   | Disp    | draw(Vx,Vy,N)| Dra sprite a coord (Vx,Vy) with width of 8 pix and hight of N pix. 
| EX9E   | KeyOp   | if(key()==Vx)| Skips next instruction if key stored in VX is pressed
| EXA1   | KeyOp   | if(key()!=Vx)| Skips the next instruction if the key sotred in Vx isn't pressed
| FX07   | Timer   | Vx=getdelay()| Set Vx to the value of the delay timer
| FX0A   | KeyOp   | Vx=getkey()  | A key press is awaited, then stored in VX. (Blocking)
| FX15   | Timer   | delaytimer(Vx)| Set delay timer to Vx
| FX18   | Sound   | soundtime(Vx)| Set sound timer to Vx
| FX1E   | MEM     | I += Vx      | Adds VX to I.
| FX29   | MEM     | I = sprite_addr[Vx] | Set I to location of srite for character in Vx, Chars 0-F are represented by a 4x5 font
                     
