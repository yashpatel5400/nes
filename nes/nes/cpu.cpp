/************************************************************************************

Filename    :   cpu.cpp
Content     :   Core CPU emulation
Authors     :   Yash Patel

8 - bit 6502 CPU running at 1.79 MHZ.It has 3 general purpose register A / X / Y, and
3 special register P(status) / SP(stack pointer) / PC(program counter, or instruction
	pointer), all of them being 8 - bit except PC which is 16 - bit.

*************************************************************************************/

#include "cpu.h"

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

void Cpu::step() {

}