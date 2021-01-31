/************************************************************************************

Filename    :   cpu.cpp
Content     :   Core CPU emulation
Authors     :   Yash Patel

8 - bit 6502 CPU running at 1.79 MHZ.It has 3 general purpose register A / X / Y, and
3 special register P(status) / SP(stack pointer) / PC(program counter, or instruction
	pointer), all of them being 8 - bit except PC which is 16 - bit.

*************************************************************************************/

#include "cpu.h"

#include <stdexcept>

Cpu::Cpu(uint8_t* memory) : memory(memory) {
}

/*

ISA (listing all the instructions that have yet to be implemented)

00: BRK impl
01: ORA X,ind
02: ---
03: ---
04: ---
05: ORA zpg
06: ASL zpg
07: ---
08: PHP impl
09: ORA #
0A: ASL A
0B: ---
0C: ---
0D: ORA abs
0E: ASL abs
0F: ---

10: BPL rel
11: ORA ind,Y
12: ---
13: ---
14: ---
15: ORA zpg,X
16: ASL zpg,X
17: ---
18: CLC impl
19: ORA abs,Y
1A: ---
1B: ---
1C: ---
1D: ORA abs,X
1E: ASL abs,X
1F: ---

20: JSR abs
21: AND X,ind
22: ---
23: ---
24: BIT zpg
25: AND zpg
26: ROL zpg
27: ---
28: PLP impl
29: AND #
2A: ROL A
2B: ---
2C: BIT abs
2D: AND abs
2E: ROL abs
2F: ---

30: BMI rel
31: AND ind,Y
32: ---
33: ---
34: ---
35: AND zpg,X
36: ROL zpg,X
37: ---
38: SEC impl
39: AND abs,Y
3A: ---
3B: ---
3C: ---
3D: AND abs,X
3E: ROL abs,X
3F: ---

40: RTI impl
41: EOR X,ind
42: ---
43: ---
44: ---
45: EOR zpg
46: LSR zpg
47: ---
48: PHA impl
49: EOR #
4A: LSR A
4B: ---
4C: JMP abs
4D: EOR abs
4E: LSR abs
4F: ---

50: BVC rel
51: EOR ind,Y
52: ---
53: ---
54: ---
55: EOR zpg,X
56: LSR zpg,X
57: ---
58: CLI impl
59: EOR abs,Y
5A: ---
5B: ---
5C: ---
5D: EOR abs,X
5E: LSR abs,X
5F: ---

60: RTS impl
61: ADC X,ind
62: ---
63: ---
64: ---
65: ADC zpg
66: ROR zpg
67: ---
68: PLA impl
69: ADC #
6A: ROR A
6B: ---
6C: JMP ind
6D: ADC abs
6E: ROR abs
6F: ---

70: BVS rel
71: ADC ind,Y
72: ---
73: ---
74: ---
75: ADC zpg,X
76: ROR zpg,X
77: ---
78: SEI impl
79: ADC abs,Y
7A: ---
7B: ---
7C: ---
7D: ADC abs,X
7E: ROR abs,X
7F: ---

80: ---
81: STA X,ind
82: ---
83: ---
84: STY zpg
85: STA zpg
86: STX zpg
87: ---
88: DEY impl
89: ---
8A: TXA impl
8B: ---
8C: STY abs
8D: STA abs
8E: STX abs
8F: ---

90: BCC rel
91: STA ind,Y
92: ---
93: ---
94: STY zpg,X
95: STA zpg,X
96: STX zpg,Y
97: ---
98: TYA impl
99: STA abs,Y
9A: TXS impl
9B: ---
9C: ---
9D: STA abs,X
9E: ---
9F: ---

A0: LDY #
A1: LDA X,ind
A2: LDX #
A3: ---
A4: LDY zpg
A5: LDA zpg
A6: LDX zpg
A7: ---
A8: TAY impl
A9: LDA #
AA: TAX impl
AB: ---
AC: LDY abs
AD: LDA abs
AE: LDX abs
AF: ---

B0: BCS rel
B1: LDA ind,Y
B2: ---
B3: ---
B4: LDY zpg,X
B5: LDA zpg,X
B6: LDX zpg,Y
B7: ---
B8: CLV impl
B9: LDA abs,Y
BA: TSX impl
BB: ---
BC: LDY abs,X
BD: LDA abs,X
BE: LDX abs,Y
BF: ---

C0: CPY #
C1: CMP X,ind
C2: ---
C3: ---
C4: CPY zpg
C5: CMP zpg
C6: DEC zpg
C7: ---
C8: INY impl
C9: CMP #
CA: DEX impl
CB: ---
CC: CPY abs
CD: CMP abs
CE: DEC abs
CF: ---

D0: BNE rel
D1: CMP ind,Y
D2: ---
D3: ---
D4: ---
D5: CMP zpg,X
D6: DEC zpg,X
D7: ---
D8: CLD impl
D9: CMP abs,Y
DA: ---
DB: ---
DC: ---
DD: CMP abs,X
DE: DEC abs,X
DF: ---

E0: CPX #
E1: SBC X,ind
E2: ---
E3: ---
E4: CPX zpg
E5: SBC zpg
E6: INC zpg
E7: ---
E8: INX impl
E9: SBC #
EA: NOP impl
EB: ---
EC: CPX abs
ED: SBC abs
EE: INC abs
EF: ---

F0: BEQ rel
F1: SBC ind,Y
F2: ---
F3: ---
F4: ---
F5: SBC zpg,X
F6: INC zpg,X
F7: ---
F8: SED impl
F9: SBC abs,Y
FA: ---
FB: ---
FC: ---
FD: SBC abs,X
FE: INC abs,X
FF: ---

*/

bool Cpu::flagNegative() {
	return bool(rsr & 0b10000000);
}

bool Cpu::flagOverflow() {
	return bool(rsr & 0b01000000);
}

bool Cpu::flagignored() {
	return bool(rsr & 0b00100000);
}

bool Cpu::flagBreak() {
	return bool(rsr & 0b00010000);
}

bool Cpu::flagDecimal() {
	return bool(rsr & 0b00001000);
}

bool Cpu::flagInterrupt() {
	return bool(rsr & 0b00000100);
}

bool Cpu::flagZero() {
	return bool(rsr & 0b00000010);
}

bool Cpu::flagCarry() {
	return bool(rsr & 0b00000001);
}

void Cpu::step() {
	uint16_t opcode;
	switch (opcode) {
	case 0x00: break;
	case 0x01: break;
	case 0x05: break;
	case 0x06: break;
	case 0x08: break;
	case 0x09: break;
	case 0x0A: break;
	case 0x0D: break;
	case 0x0E: break;
	case 0x10: break;
	case 0x11: break;
	case 0x15: break;
	case 0x16: break;
	case 0x18: break;
	case 0x19: break;
	case 0x1D: break;
	case 0x1E: break;
	case 0x20: break;
	case 0x21: break;
	case 0x24: break;
	case 0x25: break;
	case 0x26: break;
	case 0x28: break;
	case 0x29: break;
	case 0x2A: break;
	case 0x2C: break;
	case 0x2D: break;
	case 0x2E: break;
	case 0x30: break;
	case 0x31: break;
	case 0x35: break;
	case 0x36: break;
	case 0x38: break;
	case 0x39: break;
	case 0x3D: break;
	case 0x3E: break;
	case 0x40: break;
	case 0x41: break;
	case 0x45: break;
	case 0x46: break;
	case 0x48: break;
	case 0x49: break;
	case 0x4A: break;
	case 0x4C: break;
	case 0x4D: break;
	case 0x4E: break;
	case 0x50: break;
	case 0x51: break;
	case 0x55: break;
	case 0x56: break;
	case 0x58: break;
	case 0x59: break;
	case 0x5D: break;
	case 0x5E: break;
	case 0x60: break;
	case 0x61: break;
	case 0x65: break;
	case 0x66: break;
	case 0x68: break;
	case 0x69: break;
	case 0x6A: break;
	case 0x6C: break;
	case 0x6D: break;
	case 0x6E: break;
	case 0x70: break;
	case 0x71: break;
	case 0x75: break;
	case 0x76: break;
	case 0x78: break;
	case 0x79: break;
	case 0x7D: break;
	case 0x7E: break;
	case 0x81: break;
	case 0x84: break;
	case 0x85: break;
	case 0x86: break;
	case 0x88: break;
	case 0x8A: break;
	case 0x8C: break;
	case 0x8D: break;
	case 0x8E: break;
	case 0x90: break;
	case 0x91: break;
	case 0x94: break;
	case 0x95: break;
	case 0x96: break;
	case 0x98: break;
	case 0x99: break;
	case 0x9A: break;
	case 0x9D: break;
	case 0xA0: break;
	case 0xA1: break;
	case 0xA2: break;
	case 0xA4: break;
	case 0xA5: break;
	case 0xA6: break;
	case 0xA8: break;
	case 0xA9: break;
	case 0xAA: break;
	case 0xAC: break;
	case 0xAD: break;
	case 0xAE: break;
	case 0xB0: break;
	case 0xB1: break;
	case 0xB4: break;
	case 0xB5: break;
	case 0xB6: break;
	case 0xB8: break;
	case 0xB9: break;
	case 0xBA: break;
	case 0xBC: break;
	case 0xBD: break;
	case 0xBE: break;
	case 0xC0: break;
	case 0xC1: break;
	case 0xC4: break;
	case 0xC5: break;
	case 0xC6: break;
	case 0xC8: break;
	case 0xC9: break;
	case 0xCA: break;
	case 0xCC: break;
	case 0xCD: break;
	case 0xCE: break;
	case 0xD0: break;
	case 0xD1: break;
	case 0xD5: break;
	case 0xD6: break;
	case 0xD8: break;
	case 0xD9: break;
	case 0xDD: break;
	case 0xDE: break;
	case 0xE0: break;
	case 0xE1: break;
	case 0xE4: break;
	case 0xE5: break;
	case 0xE6: break;
	case 0xE8: break;
	case 0xE9: break;
	case 0xEA: break;
	case 0xEC: break;
	case 0xED: break;
	case 0xEE: break;
	case 0xF0: break;
	case 0xF1: break;
	case 0xF5: break;
	case 0xF6: break;
	case 0xF8: break;
	case 0xF9: break;
	case 0xFD: break;
	case 0xFE: break;
	default:
		throw std::runtime_error("Unexpected opcode: " + opcode);
	}
}