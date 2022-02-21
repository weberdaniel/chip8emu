# Chip8emu


CHIP-8 is an interpreted programming language developed by Joseph Weisbecker. It was used on the 
CoSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s

## Opcodes


| Opcode | Done    | Type    | Pseudocode   | Action
|--------|---------|---------|--------------|-----------------------------------
| 0NNN   | ign     | Call    |              | ignored by modern interpreters
| 00E0   | ok      | Display |              | Clears the screen
| 00EE   | ok      | Flow    | return       | Returns from a subroutine
| 1NNN   | ok      | Flow    | goto NNN     | Jump to Address NNN
| 2NNN   | ok      | Flow    | \*(0xNNN)()  | Calls subroutine at NNN
| 3XNN   | ok      | Cond    | if(Vx==NN)() | Skip the next instruction if Vx eq NN
| 4XNN   | ok      | Cond    | if(Vx!=NN)   | Skips next instruction if VX doesn't equal
| 5XY0   | ok      | Cond    | if(Vx==Vy)   | Skips the next instruction if Vx equals Vy
| 6XNN   | ok      | Const   | Vx = NN      | Sets Vx to NN
| 7XNN   | ok      | Const   | Vx += NN     | Adds NN to VX. (Carry flag not changed)
| 8XY0   | ok      | Assign  | Vx=Vy        | Sets VX to the value of VY
| 8XY1   | ok      | BitOp   | Vx=Vx\|Vy    | Sets VX to VX or VY
| 8XY2   | ok      | BitOp   | Vx=Vx&Vy     | Sets VX to VX and VY
| 8XY3   | ok      | BitOp   | Vx=Vx^Vy     | Sets VX to VX xor VY
| 8XY4   | ok      | Math    | Vx += Vy     | Adds VY to Vx. VF is set to 1 if there is a carry, else to 0.
| 8XY5   | ok      | Math    | Vx -= Vy     | VY is substracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
| 8XY6   | ok      | BitOp   | Vx >>= 1     | Store the least significant bit of VX in VF and shift VX to right by 1
| 8XY7   | ok      | Math    | Vx=Vy-Vx     | Set VX to VY minus VX. VF is set to 0 when there's a borrow
| 8XYE   | ok      | BitOp   | Vx<<=1       | Store MSB of VX in VF and shift VX to left by 1
| 9XY0   | ok      | Cond    | if(Vx!=Vy)   | Skip next instruction if VX doesn't equal VY
| ANNN   | ok      | MEM     | 1 = NNN      | Sets I to the address of NNN
| BNNN   | ok      | Flow    | PC=V0+NNN    | Jump to address NNN plus V0
| CXNN   | ok      | Rand    | Vx=rand()&NN | Sets Vx to the result of a bitwise and op. on a random number and NN
| DXYN   | ok      | Disp    | draw(Vx,Vy,N)| Dra sprite a coord (Vx,Vy) with width of 8 pix and hight of N pix. 
| EX9E   | ok      | KeyOp   | if(key()==Vx)| Skips next instruction if key stored in VX is pressed
| EXA1   | ok      | KeyOp   | if(key()!=Vx)| Skips the next instruction if the key sotred in Vx isn't pressed
| FX07   | ok      | Timer   | Vx=getdelay()| Set Vx to the value of the delay timer
| FX0A   | ok      | KeyOp   | Vx=getkey()  | A key press is awaited, then stored in VX. (Blocking)
| FX15   | ok      | Timer   | delaytimer(Vx)| Set delay timer to Vx
| FX18   | ok      | Sound   | soundtime(Vx)| Set sound timer to Vx
| FX1E   | ok      | MEM     | I += Vx      | Adds VX to I.
| FX29   | ok      | MEM     | I = spriteaddr[Vx] | Set I to location of srite for character in Vx, Chars 0-F are represented by a 4x5 font
| FX33   | tbt     | BCD     |              | Store binary-coded decimal rep of Vx at position I
| FX55   | tbt     | MEM     | regdump      | Store V0 to VX starting at I. offset from i is increased by 1 for each value
| FX65   | tbt     | MEM     | regload      | Fill V0 to Fx with values from memory starting at I. increase offset from I but do not touch I

## Registers

The CHIP-8 has 16, 8-bit virtual registers: V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF.
There is one single 16-bit register: I which is used for reading and writing to memory.
There are two 8-bit timer registers that continuously count down at 60 Hz. The delay timer is good for time limiting
the game of waiting brief periods. While the sound timer is non-zero a tone will be emitted.
Contains 8 8bit user-flag registers R0-R7. Cannot be directly used, but registers V0-V7 can be saved and
loaded from. 
