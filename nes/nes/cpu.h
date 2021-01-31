/************************************************************************************

Filename    :   cpu.h
Content     :   Core CPU emulation (header)
Authors     :   Yash Patel

8 - bit 6502 CPU running at 1.79 MHZ.It has 3 general purpose register A / X / Y, and
3 special register P(status) / SP(stack pointer) / PC(program counter, or instruction
	pointer), all of them being 8 - bit except PC which is 16 - bit.

*************************************************************************************/

#include <stdint.h>

class Cpu {
public:
	Cpu(uint8_t* memory);
	~Cpu() = default;

	void step();

private: 
	uint8_t* memory;

	uint8_t ra, rx, ry;
	uint8_t rp, rsp;
	uint16_t rpc;
};