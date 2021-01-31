/************************************************************************************

Filename    :   main.cpp
Content     :   Main entrypoint for NES emulator. Overall architecture of system laid
                in core code below
Authors     :   Yash Patel

The NES consists of the following components, each emulated separately
and tied together in this main function. Copied from https://yizhang82.dev/nes-emu-overview,
the specs are as follows:

"8-bit 6502 CPU running at 1.79 MHZ. It has 3 general purpose register A/X/Y, and 
3 special register P (status) /SP (stack pointer) /PC (program counter, or instruction 
pointer), all of them being 8-bit except PC which is 16-bit.

16-bit addressable memory space. In theory it can address 64K memory however it only
has 2KB onboard RAM. Rest is either not wired up (and are mirrors of those 2KB), or 
mapped to special I/O registers, or catridge ROM/RAM space.

PPU (Picture Processing Unit) supporting rendering 256x240 screen composed of 8x8 tiles 
for background, up to 64 8x8 or 8x16 sprites for moving objects. It supports pixel-level 
scrolling (which is a big deal back in that day).

APU (Audio Processing Unit) supporting 2 pulse channel, 1 triangle channel, 1 noise channel, 
and 1 DMC (delta modulation) channel. 
Controllers - from classic NES controller to NES mouse.

Catridge boards (and mappers) - there are many different kinds of game catridge boards. 
They come with game data as ROMsm, sometimes their own battery-backed RAM, or some
cases, their own audio processing unit. Most importantly, they also come with special 
hardware, refered to as mappers, that dynamically maps ROM/RAM into CPU and PPU memory 
space, bypassing the limitation of 16-bit address space. Some game catridge come with 
more than 256KB of CHR ROM and swap/map portion of it on demand."

Note: If you are looking at this code in the middle of its development, you will likely
find some of the components either not completed, broken, or both.

*************************************************************************************/

#include <conio.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "cpu.h"

int main() {
	const int kMemorySize = 65536;
	uint8_t memory[kMemorySize]; // 6502 has 16 address lines, meaning it can address 2^16 bytes
	std::fill(memory, memory + kMemorySize, 0x00);

	// TODO: load memory manually for now -- we will obviously load in actual ROMs in the end
	memory[0x6000] = 0xa9;
	memory[0x6001] = 0x01;
	memory[0x6002] = 0x8d;
	memory[0x6003] = 0x00;
	memory[0x6004] = 0x02;
	memory[0x6005] = 0xa9;
	memory[0x6006] = 0x05;
	memory[0x6007] = 0x8d;
	memory[0x6008] = 0x01;
	memory[0x6009] = 0x02;
	memory[0x600A] = 0xa9;
	memory[0x600B] = 0x08;
	memory[0x600C] = 0x8d;
	memory[0x600D] = 0x02;
	memory[0x600E] = 0x02;

	// 6502 has a reset vector of FFFC and FFFD and also is little endian ==> 00 60 is 0x6000
	memory[0xFFFC] = 0x00;
	memory[0xFFFD] = 0x60;

	CPU cpu(memory);

	char control; // just used for stepping for now
	while (true) {
		control = _getch();
		     if (control == ' ') { cpu.step(); }
		else if (control == 'd') { cpu.dump(); }
		else { continue; }
	}

	return 0;
}