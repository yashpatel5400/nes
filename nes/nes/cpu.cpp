/************************************************************************************

Filename    :   CPU.cpp
Content     :   Core CPU emulation
Authors     :   Yash Patel

Note: All credits for documentation/comments provided below are to:
https://www.masswerk.at/6502/6502_instruction_set.html. This is a tremendous
resource if you are interested in implementing the 6502 yourself (or understanding it)!

*************************************************************************************/

#include "CPU.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

uint16_t CPU::readShort(uint16_t addr) {
    uint16_t ll = memory[rpc++];
    uint16_t hh = memory[rpc++];
    return (hh << 8) | ll;
}

CPU::CPU(uint8_t* memory) : memory(memory) {
    rpc = readShort(0xFFFC); // program counter starts w/ value at FFFC
    rac = 0;  // accumulator (8 bit)
    rx = 0;   // X register  (8 bit)
    ry = 0;   // Y register  (8 bit)
    rsr = 0;  // status register [NV-BDIZC]  (8 bit)
    rsp = 0;  // stack pointer   (8 bit)
}

/************************************************************************************

SR Flags (bit 7 to bit 0):

N	....	Negative
V	....	Overflow
-	....	ignored
B	....	Break
D	....	Decimal (use BCD for arithmetics)
I	....	Interrupt (IRQ disable)
Z	....	Zero
C	....	Carry

*************************************************************************************/

bool CPU::srN() {
    return bool(rsr & 0b10000000);
}

bool CPU::srV() {
    return bool(rsr & 0b01000000);
}

bool CPU::srB() {
    return bool(rsr & 0b00010000);
}

bool CPU::srD() {
    return bool(rsr & 0b00001000);
}

bool CPU::srI() {
    return bool(rsr & 0b00000100);
}

bool CPU::srZ() {
    return bool(rsr & 0b00000010);
}

bool CPU::srC() {
    return bool(rsr & 0b00000001);
}

/************************************************************************************

Helper methods for memory access that are used repeatedly throughout the code:

Address Modes:

A		....	Accumulator	 	        OPC A	 	    operand is AC (implied single byte instruction)
abs		....	absolute	 	        OPC $LLHH	 	operand is address $HHLL *
abs,X	....	absolute, X-indexed	 	OPC $LLHH,X	 	operand is address; effective address is address incremented by X with carry **
abs,Y	....	absolute, Y-indexed	 	OPC $LLHH,Y	 	operand is address; effective address is address incremented by Y with carry **
#		....	immediate	 	        OPC #$BB	 	operand is byte BB
impl	....	implied	 	            OPC	 	        operand implied
ind		....	indirect	 	        OPC ($LLHH)	 	operand is address; effective address is contents of word at address: C.w($HHLL)
X,ind	....	X-indexed, indirect	 	OPC ($LL,X)	 	operand is zeropage address; effective address is word in (LL + X, LL + X + 1), inc. without carry: C.w($00LL + X)
ind,Y	....	indirect, Y-indexed	 	OPC ($LL),Y	 	operand is zeropage address; effective address is word in (LL, LL + 1) incremented by Y with carry: C.w($00LL) + Y
rel		....	relative	 	        OPC $BB	 	    branch target is PC + signed offset BB ***
zpg		....	zeropage	 	        OPC $LL	 	    operand is zeropage address (hi-byte is zero, address = $00LL)
zpg,X	....	zeropage, X-indexed	 	OPC $LL,X	 	operand is zeropage address; effective address is address incremented by X without carry **
zpg,Y	....	zeropage, Y-indexed	 	OPC $LL,Y	 	operand is zeropage address; effective address is address incremented by Y without carry **


*************************************************************************************/

uint8_t CPU::operandAcc() {
    return rac;
}

uint16_t CPU::operandAbs() {
    uint16_t hhll = readShort(rpc);
    return hhll;
}

uint16_t CPU::operandAbsX() {
    uint16_t hhll = readShort(rpc);
    return hhll + rx;
}

uint16_t CPU::operandAbsY() {
    uint16_t hhll = readShort(rpc);
    return hhll + ry;
}

uint8_t CPU::operandImm() {
    uint8_t bb = memory[rpc++];
    return bb;
}

uint16_t CPU::operandInd() {
    uint16_t hhll = readShort(rpc);
    return memory[hhll];
}

uint8_t CPU::operandIndX() {
    uint16_t ll = memory[rpc++];
    return memory[ll + rx];
}

uint8_t CPU::operandIndY() {
    uint16_t ll = memory[rpc++];
    return memory[ll] + ry;
}

uint8_t CPU::operandRelative() {
    return 0;
}

uint8_t CPU::operandZpg() {
    uint16_t ll = memory[rpc++];
    return ll;
}

uint8_t CPU::operandZpgX() {
    uint16_t ll = memory[rpc++];
    return ll + rx;
}

uint8_t CPU::operandZpgX() {
    uint16_t ll = memory[rpc++];
    return ll + ry;
}

/************************************************************************************

ADC  Add Memory to Accumulator with Carry

     A + M + C -> A, C                N Z C I D V
                                      + + + - - +

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     ADC #oper     69    2     2
     zeropage      ADC oper      65    2     3
     zeropage,X    ADC oper,X    75    2     4
     absolute      ADC oper      6D    3     4
     absolute,X    ADC oper,X    7D    3     4*
     absolute,Y    ADC oper,Y    79    3     4*
     (indirect,X)  ADC (oper,X)  61    2     6
     (indirect),Y  ADC (oper),Y  71    2     5*

*************************************************************************************/
void CPU::ADC(uint16_t opcode) { //add with carry
    switch (opcode) {
    case 0x69: {
        rac += operandAbs();
        break;
    }
    case 0x65: {
        rac += operandZpg();
        break;
    }
    case 0x75: {
        rac += operandZpgX();
        break;
    }
    case 0x6D: {
        rac += operandAbs();
        break;
    }
    case 0x7D: {
        rac += operandAbsX();
        break;
    }
    case 0x79: {
        rac += operandAbsY();
        break;
    }
    case 0x61: {
        rac += operandIndX();
        break;
    }
    case 0x71: {
        rac += operandIndY();
        break;
    }
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

AND  AND Memory with Accumulator

     A AND M -> A                     N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     AND #oper     29    2     2
     zeropage      AND oper      25    2     3
     zeropage,X    AND oper,X    35    2     4
     absolute      AND oper      2D    3     4
     absolute,X    AND oper,X    3D    3     4*
     absolute,Y    AND oper,Y    39    3     4*
     (indirect,X)  AND (oper,X)  21    2     6
     (indirect),Y  AND (oper),Y  31    2     5*

*************************************************************************************/
void CPU::AND(uint16_t opcode) { //and (with accumulator)
    switch (opcode) {
    case 0x29: break;
    case 0x25: break;
    case 0x35: break;
    case 0x2D: break;
    case 0x3D: break;
    case 0x39: break;
    case 0x21: break;
    case 0x31: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}


/************************************************************************************

ASL  Shift Left One Bit (Memory or Accumulator)

     C <- [76543210] <- 0             N Z C I D V
                                      + + + - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     accumulator   ASL A         0A    1     2
     zeropage      ASL oper      06    2     5
     zeropage,X    ASL oper,X    16    2     6
     absolute      ASL oper      0E    3     6
     absolute,X    ASL oper,X    1E    3     7

*************************************************************************************/
void CPU::ASL(uint16_t opcode) { //arithmetic shift left
    switch (opcode) {
    case 0x0A: break;
    case 0x06: break;
    case 0x16: break;
    case 0x0E: break;
    case 0x1E: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BCC  Branch on Carry Clear

     branch on C = 0                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BCC oper      90    2     2**

*************************************************************************************/
void CPU::BCC(uint16_t opcode) { //branch on carry clear
    switch (opcode) {
    case 0x90: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BCS  Branch on Carry Set

     branch on C = 1                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BCS oper      B0    2     2**

*************************************************************************************/
void CPU::BCS(uint16_t opcode) { //branch on carry set
    switch (opcode) {
    case 0xB0: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BEQ  Branch on Result Zero

     branch on Z = 1                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BEQ oper      F0    2     2**

*************************************************************************************/
void CPU::BEQ(uint16_t opcode) { //branch on equal (zero set)
    switch (opcode) {
    case 0xF0: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BIT  Test Bits in Memory with Accumulator

     bits 7 and 6 of operand are transfered to bit 7 and 6 of SR (N,V);
     the zeroflag is set to the result of operand AND accumulator.

     A AND M, M7 -> N, M6 -> V        N Z C I D V
                                     M7 + - - - M6

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      BIT oper      24    2     3
     absolute      BIT oper      2C    3     4

*************************************************************************************/
void CPU::BIT(uint16_t opcode) { //bit test
    switch (opcode) {
    case 0x24: break;
    case 0x2C: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BMI  Branch on Result Minus

     branch on N = 1                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BMI oper      30    2     2**

*************************************************************************************/
void CPU::BMI(uint16_t opcode) { //branch on minus (negative set)
    switch (opcode) {
    case 0x30: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BNE  Branch on Result not Zero

     branch on Z = 0                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BNE oper      D0    2     2**

*************************************************************************************/
void CPU::BNE(uint16_t opcode) { //branch on not equal (zero clear)
    switch (opcode) {
    case 0xD0: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BPL  Branch on Result Plus

     branch on N = 0                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BPL oper      10    2     2**

*************************************************************************************/
void CPU::BPL(uint16_t opcode) { //branch on plus (negative clear)
    switch (opcode) {
    case 0x10: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BRK  Force Break

     interrupt,                       N Z C I D V
     push PC+2, push SR               - - - 1 - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       BRK           00    1     7

*************************************************************************************/
void CPU::BRK(uint16_t opcode) { //break / interrupt
    switch (opcode) {
    case 0x00: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BVC  Branch on Overflow Clear

     branch on V = 0                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BVC oper      50    2     2**

*************************************************************************************/
void CPU::BVC(uint16_t opcode) { //branch on overflow clear
    switch (opcode) {
    case 0x50: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

BVS  Branch on Overflow Set

     branch on V = 1                  N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     relative      BVC oper      70    2     2**

*************************************************************************************/
void CPU::BVS(uint16_t opcode) { //branch on overflow set
    switch (opcode) {
    case 0x70: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

CLC  Clear Carry Flag

     0 -> C                           N Z C I D V
                                      - - 0 - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       CLC           18    1     2

*************************************************************************************/
void CPU::CLC(uint16_t opcode) { //clear carry
    switch (opcode) {
    case 0x18: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

CLD  Clear Decimal Mode

     0 -> D                           N Z C I D V
                                      - - - - 0 -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       CLD           D8    1     2

*************************************************************************************/
void CPU::CLD(uint16_t opcode) { //clear decimal
    switch (opcode) {
    case 0xD8: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

CLI  Clear Interrupt Disable Bit

     0 -> I                           N Z C I D V
                                      - - - 0 - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       CLI           58    1     2

*************************************************************************************/
void CPU::CLI(uint16_t opcode) { //clear interrupt disable
    switch (opcode) {
    case 0x58: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

CLV  Clear Overflow Flag

     0 -> V                           N Z C I D V
                                      - - - - - 0

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       CLV           B8    1     2

*************************************************************************************/
void CPU::CLV(uint16_t opcode) { //clear overflow
    switch (opcode) {
    case 0xB8: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

CMP  Compare Memory with Accumulator

     A - M                            N Z C I D V
                                    + + + - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     CMP #oper     C9    2     2
     zeropage      CMP oper      C5    2     3
     zeropage,X    CMP oper,X    D5    2     4
     absolute      CMP oper      CD    3     4
     absolute,X    CMP oper,X    DD    3     4*
     absolute,Y    CMP oper,Y    D9    3     4*
     (indirect,X)  CMP (oper,X)  C1    2     6
     (indirect),Y  CMP (oper),Y  D1    2     5*

*************************************************************************************/
void CPU::CMP(uint16_t opcode) { //compare (with accumulator)
    switch (opcode) {
    case 0xC9: break;
    case 0xC5: break;
    case 0xD5: break;
    case 0xCD: break;
    case 0xDD: break;
    case 0xD9: break;
    case 0xC1: break;
    case 0xD1: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

CPX  Compare Memory and Index X

     X - M                            N Z C I D V
                                      + + + - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     CPX #oper     E0    2     2
     zeropage      CPX oper      E4    2     3
     absolute      CPX oper      EC    3     4

*************************************************************************************/
void CPU::CPX(uint16_t opcode) { //compare with X
    switch (opcode) {
    case 0xE0: break;
    case 0xE4: break;
    case 0xEC: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

CPY  Compare Memory and Index Y

     Y - M                            N Z C I D V
                                      + + + - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     CPY #oper     C0    2     2
     zeropage      CPY oper      C4    2     3
     absolute      CPY oper      CC    3     4

*************************************************************************************/
void CPU::CPY(uint16_t opcode) { //compare with Y
    switch (opcode) {
    case 0xC0: break;
    case 0xC4: break;
    case 0xCC: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

DEC  Decrement Memory by One

     M - 1 -> M                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      DEC oper      C6    2     5
     zeropage,X    DEC oper,X    D6    2     6
     absolute      DEC oper      CE    3     6
     absolute,X    DEC oper,X    DE    3     7

*************************************************************************************/
void CPU::DEC(uint16_t opcode) { //decrement
    switch (opcode) {
    case 0xC6: break;
    case 0xD6: break;
    case 0xCE: break;
    case 0xDE: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

DEX  Decrement Index X by One

     X - 1 -> X                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       DEC           CA    1     2

*************************************************************************************/
void CPU::DEX(uint16_t opcode) { //decrement X
    switch (opcode) {
    case 0xCA: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

DEY  Decrement Index Y by One

     Y - 1 -> Y                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       DEC           88    1     2

*************************************************************************************/
void CPU::DEY(uint16_t opcode) { //decrement Y
    switch (opcode) {
    case 0x88: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

EOR  Exclusive-OR Memory with Accumulator

     A EOR M -> A                     N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     EOR #oper     49    2     2
     zeropage      EOR oper      45    2     3
     zeropage,X    EOR oper,X    55    2     4
     absolute      EOR oper      4D    3     4
     absolute,X    EOR oper,X    5D    3     4*
     absolute,Y    EOR oper,Y    59    3     4*
     (indirect,X)  EOR (oper,X)  41    2     6
     (indirect),Y  EOR (oper),Y  51    2     5*

*************************************************************************************/
void CPU::EOR(uint16_t opcode) { //exclusive or (with accumulator)
    switch (opcode) {
    case 0x49: break;
    case 0x45: break;
    case 0x55: break;
    case 0x4D: break;
    case 0x5D: break;
    case 0x59: break;
    case 0x41: break;
    case 0x51: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

INC  Increment Memory by One

     M + 1 -> M                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      INC oper      E6    2     5
     zeropage,X    INC oper,X    F6    2     6
     absolute      INC oper      EE    3     6
     absolute,X    INC oper,X    FE    3     7

*************************************************************************************/
void CPU::INC(uint16_t opcode) { //increment
    switch (opcode) {
    case 0xE6: break;
    case 0xF6: break;
    case 0xEE: break;
    case 0xFE: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

INX  Increment Index X by One

     X + 1 -> X                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       INX           E8    1     2

*************************************************************************************/
void CPU::INX(uint16_t opcode) { //increment X
    switch (opcode) {
    case 0xE8: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

INY  Increment Index Y by One

     Y + 1 -> Y                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       INY           C8    1     2

*************************************************************************************/
void CPU::INY(uint16_t opcode) { //increment Y
    switch (opcode) {
    case 0xC8: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

JMP  Jump to New Location

     (PC+1) -> PCL                    N Z C I D V
     (PC+2) -> PCH                    - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     absolute      JMP oper      4C    3     3
     indirect      JMP (oper)    6C    3     5

*************************************************************************************/
void CPU::JMP(uint16_t opcode) { //jump
    switch (opcode) {
    case 0x4C: break;
    case 0x6C: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

JSR  Jump to New Location Saving Return Address

     push (PC+2),                     N Z C I D V
     (PC+1) -> PCL                    - - - - - -
     (PC+2) -> PCH

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     absolute      JSR oper      20    3     6

*************************************************************************************/
void CPU::JSR(uint16_t opcode) { //jump subroutine
    switch (opcode) {
    case 0x20: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

LDA  Load Accumulator with Memory

     M -> A                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     LDA #oper     A9    2     2
     zeropage      LDA oper      A5    2     3
     zeropage,X    LDA oper,X    B5    2     4
     absolute      LDA oper      AD    3     4
     absolute,X    LDA oper,X    BD    3     4*
     absolute,Y    LDA oper,Y    B9    3     4*
     (indirect,X)  LDA (oper,X)  A1    2     6
     (indirect),Y  LDA (oper),Y  B1    2     5*

*************************************************************************************/
void CPU::LDA(uint16_t opcode) { //load accumulator
    switch (opcode) {
    case 0xA9: {
        uint16_t arg = readShort(rpc);
        rpc += 2;
        rac += memory[arg];
        break;
    }
    case 0xA5: break;
    case 0xB5: break;
    case 0xAD: break;
    case 0xBD: break;
    case 0xB9: break;
    case 0xA1: break;
    case 0xB1: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

LDX  Load Index X with Memory

     M -> X                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     LDX #oper     A2    2     2
     zeropage      LDX oper      A6    2     3
     zeropage,Y    LDX oper,Y    B6    2     4
     absolute      LDX oper      AE    3     4
     absolute,Y    LDX oper,Y    BE    3     4*

*************************************************************************************/
void CPU::LDX(uint16_t opcode) { //load X
    switch (opcode) {
    case 0xA2: break;
    case 0xA6: break;
    case 0xB6: break;
    case 0xAE: break;
    case 0xBE: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

LDY  Load Index Y with Memory

     M -> Y                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     LDY #oper     A0    2     2
     zeropage      LDY oper      A4    2     3
     zeropage,X    LDY oper,X    B4    2     4
     absolute      LDY oper      AC    3     4
     absolute,X    LDY oper,X    BC    3     4*

*************************************************************************************/
void CPU::LDY(uint16_t opcode) { //load Y
    switch (opcode) {
    case 0xA0: break;
    case 0xA4: break;
    case 0xB4: break;
    case 0xAC: break;
    case 0xBC: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

LSR  Shift One Bit Right (Memory or Accumulator)

     0 -> [76543210] -> C             N Z C I D V
                                      0 + + - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     accumulator   LSR A         4A    1     2
     zeropage      LSR oper      46    2     5
     zeropage,X    LSR oper,X    56    2     6
     absolute      LSR oper      4E    3     6
     absolute,X    LSR oper,X    5E    3     7

*************************************************************************************/
void CPU::LSR(uint16_t opcode) { //logical shift right
    switch (opcode) {
    case 0x4A: break;
    case 0x46: break;
    case 0x56: break;
    case 0x4E: break;
    case 0x5E: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

NOP  No Operation

     ---                              N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       NOP           EA    1     2

*************************************************************************************/
void CPU::NOP(uint16_t opcode) { //no operation
    switch (opcode) {
    case 0xEA: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

ORA  OR Memory with Accumulator

     A OR M -> A                      N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     ORA #oper     09    2     2
     zeropage      ORA oper      05    2     3
     zeropage,X    ORA oper,X    15    2     4
     absolute      ORA oper      0D    3     4
     absolute,X    ORA oper,X    1D    3     4*
     absolute,Y    ORA oper,Y    19    3     4*
     (indirect,X)  ORA (oper,X)  01    2     6
     (indirect),Y  ORA (oper),Y  11    2     5*

*************************************************************************************/
void CPU::ORA(uint16_t opcode) { //or with accumulator
    switch (opcode) {
    case 0x09: break;
    case 0x05: break;
    case 0x15: break;
    case 0x0D: break;
    case 0x1D: break;
    case 0x19: break;
    case 0x01: break;
    case 0x11: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

PHA  Push Accumulator on Stack

     push A                           N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       PHA           48    1     3

*************************************************************************************/
void CPU::PHA(uint16_t opcode) { //push accumulator
    switch (opcode) {
    case 0x48: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

PHP  Push Processor Status on Stack

     push SR                          N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       PHP           08    1     3

*************************************************************************************/
void CPU::PHP(uint16_t opcode) { //push processor status (SR)
    switch (opcode) {
    case 0x08: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

PLA  Pull Accumulator from Stack

     pull A                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       PLA           68    1     4

*************************************************************************************/
void CPU::PLA(uint16_t opcode) { //pull accumulator
    switch (opcode) {
    case 0x68: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

PLP  Pull Processor Status from Stack

     pull SR                          N Z C I D V
                                      from stack

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       PLP           28    1     4

*************************************************************************************/
void CPU::PLP(uint16_t opcode) { //pull processor status (SR)
    switch (opcode) {
    case 0x28: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

ROL  Rotate One Bit Left (Memory or Accumulator)

     C <- [76543210] <- C             N Z C I D V
                                      + + + - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     accumulator   ROL A         2A    1     2
     zeropage      ROL oper      26    2     5
     zeropage,X    ROL oper,X    36    2     6
     absolute      ROL oper      2E    3     6
     absolute,X    ROL oper,X    3E    3     7

*************************************************************************************/
void CPU::ROL(uint16_t opcode) { //rotate left
    switch (opcode) {
    case 0x2A: break;
    case 0x26: break;
    case 0x36: break;
    case 0x2E: break;
    case 0x3E: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

ROR  Rotate One Bit Right (Memory or Accumulator)

     C -> [76543210] -> C             N Z C I D V
                                      + + + - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     accumulator   ROR A         6A    1     2
     zeropage      ROR oper      66    2     5
     zeropage,X    ROR oper,X    76    2     6
     absolute      ROR oper      6E    3     6
     absolute,X    ROR oper,X    7E    3     7

*************************************************************************************/
void CPU::ROR(uint16_t opcode) { //rotate right
    switch (opcode) {
    case 0x6A: break;
    case 0x66: break;
    case 0x76: break;
    case 0x6E: break;
    case 0x7E: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

RTI  Return from Interrupt

     pull SR, pull PC                 N Z C I D V
                                      from stack

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       RTI           40    1     6

*************************************************************************************/
void CPU::RTI(uint16_t opcode) { //return from interrupt
    switch (opcode) {
    case 0x40: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

RTS  Return from Subroutine

     pull PC, PC+1 -> PC              N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       RTS           60    1     6

*************************************************************************************/
void CPU::RTS(uint16_t opcode) { //return from subroutine
    switch (opcode) {
    case 0x60: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

SBC  Subtract Memory from Accumulator with Borrow

     A - M - C -> A                   N Z C I D V
                                      + + + - - +

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     immidiate     SBC #oper     E9    2     2
     zeropage      SBC oper      E5    2     3
     zeropage,X    SBC oper,X    F5    2     4
     absolute      SBC oper      ED    3     4
     absolute,X    SBC oper,X    FD    3     4*
     absolute,Y    SBC oper,Y    F9    3     4*
     (indirect,X)  SBC (oper,X)  E1    2     6
     (indirect),Y  SBC (oper),Y  F1    2     5*

*************************************************************************************/
void CPU::SBC(uint16_t opcode) { //subtract with carry
    switch (opcode) {
    case 0xE9: break;
    case 0xE5: break;
    case 0xF5: break;
    case 0xED: break;
    case 0xFD: break;
    case 0xF9: break;
    case 0xE1: break;
    case 0xF1: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

SEC  Set Carry Flag

     1 -> C                           N Z C I D V
                                      - - 1 - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       SEC           38    1     2

*************************************************************************************/
void CPU::SEC(uint16_t opcode) { //set carry
    switch (opcode) {
    case 0x38: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

SED  Set Decimal Flag

     1 -> D                           N Z C I D V
                                      - - - - 1 -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       SED           F8    1     2

*************************************************************************************/
void CPU::SED(uint16_t opcode) { //set decimal
    switch (opcode) {
    case 0xF8: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

SEI  Set Interrupt Disable Status

     1 -> I                           N Z C I D V
                                      - - - 1 - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       SEI           78    1     2

*************************************************************************************/
void CPU::SEI(uint16_t opcode) { //set interrupt disable
    switch (opcode) {
    case 0x78: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

STA  Store Accumulator in Memory

     A -> M                           N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      STA oper      85    2     3
     zeropage,X    STA oper,X    95    2     4
     absolute      STA oper      8D    3     4
     absolute,X    STA oper,X    9D    3     5
     absolute,Y    STA oper,Y    99    3     5
     (indirect,X)  STA (oper,X)  81    2     6
     (indirect),Y  STA (oper),Y  91    2     6

*************************************************************************************/
void CPU::STA(uint16_t opcode) { //store accumulator
    switch (opcode) {
    case 0x85: break;
    case 0x95: break;
    case 0x8D: break;
    case 0x9D: break;
    case 0x99: break;
    case 0x81: break;
    case 0x91: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

STX  Store Index X in Memory

     X -> M                           N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      STX oper      86    2     3
     zeropage,Y    STX oper,Y    96    2     4
     absolute      STX oper      8E    3     4

*************************************************************************************/
void CPU::STX(uint16_t opcode) { //store X
    switch (opcode) {
    case 0x86: break;
    case 0x96: break;
    case 0x8E: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

STY  Store Index Y in Memory

     Y -> M                           N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      STY oper      84    2     3
     zeropage,X    STY oper,X    94    2     4
     absolute      STY oper      8C    3     4

*************************************************************************************/
void CPU::STY(uint16_t opcode) { //store Y
    switch (opcode) {
    case 0x84: break;
    case 0x94: break;
    case 0x8C: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

TAX  Transfer Accumulator to Index X

     A -> X                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TAX           AA    1     2

*************************************************************************************/
void CPU::TAX(uint16_t opcode) { //transfer accumulator to X
    switch (opcode) {
    case 0xAA: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

TAY  Transfer Accumulator to Index Y

     A -> Y                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TAY           A8    1     2

*************************************************************************************/
void CPU::TAY(uint16_t opcode) { //transfer accumulator to Y
    switch (opcode) {
    case 0xA8: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

TSX  Transfer Stack Pointer to Index X

     SP -> X                          N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TSX           BA    1     2

*************************************************************************************/
void CPU::TSX(uint16_t opcode) { //transfer stack pointer to X
    switch (opcode) {
    case 0xBA: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

TXA  Transfer Index X to Accumulator

     X -> A                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TXA           8A    1     2

*************************************************************************************/
void CPU::TXA(uint16_t opcode) { //transfer X to accumulator
    switch (opcode) {
    case 0x8A: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

TXS  Transfer Index X to Stack Register

     X -> SP                          N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TXS           9A    1     2

*************************************************************************************/
void CPU::TXS(uint16_t opcode) { //transfer X to stack pointer
    switch (opcode) {
    case 0x9A: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

/************************************************************************************

TYA  Transfer Index Y to Accumulator

     Y -> A                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TYA           98    1     2

*************************************************************************************/
void CPU::TYA(uint16_t opcode) { //transfer Y to accumulator
    switch (opcode) {
    case 0x98: break;
    default: throw std::runtime_error("Incorrect dispatch: " + opcode);
    }
}

const std::string separator = "----------------------------------";

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

template <typename T>
void printHex(const std::string& name, T opcode) {
    std::string opcodeStr = string_format("%04x", opcode);
    const int centeringSpaces = (separator.size() - opcodeStr.size() - name.size() - 3) / 2;
    const std::string centering(centeringSpaces, ' ');
    std::cout << "[" << centering
        << name << " " << std::hex << std::setfill('0') << std::setw(2) << opcodeStr
        << centering << "]" << std::endl;
}

void CPU::dump() {
    std::cout << "[           6502 State           ]" << std::endl;
    std::cout << separator << std::endl;
    printHex("[OPC]", opcode);
    std::cout << separator << std::endl;
    printHex("[REG]PC", rpc);
    printHex("[REG]AC", rac);
    printHex("[REG] X", rx);
    printHex("[REG] Y", ry);
    printHex("[REG]SR", rsr);
    printHex("[REG]SP", rsp);
    std::cout << separator << std::endl;
}

void CPU::step() {
    uint8_t opcode = memory[rpc++];
//    uint16_t byte = rawByte; // need to reassign to allow shift
//    opcode = (byte << 8) | (opcode >> 8);

    switch (opcode) {
    case 0x00: BRK(opcode); break;
    case 0x01: ORA(opcode); break;
    case 0x05: ORA(opcode); break;
    case 0x06: ASL(opcode); break;
    case 0x08: PHP(opcode); break;
    case 0x09: ORA(opcode); break;
    case 0x0A: ASL(opcode); break;
    case 0x0D: ORA(opcode); break;
    case 0x0E: ASL(opcode); break;
    case 0x10: BPL(opcode); break;
    case 0x11: ORA(opcode); break;
    case 0x15: ORA(opcode); break;
    case 0x16: ASL(opcode); break;
    case 0x18: CLC(opcode); break;
    case 0x19: ORA(opcode); break;
    case 0x1D: ORA(opcode); break;
    case 0x1E: ASL(opcode); break;
    case 0x20: JSR(opcode); break;
    case 0x21: AND(opcode); break;
    case 0x24: BIT(opcode); break;
    case 0x25: AND(opcode); break;
    case 0x26: ROL(opcode); break;
    case 0x28: PLP(opcode); break;
    case 0x29: AND(opcode); break;
    case 0x2A: ROL(opcode); break;
    case 0x2C: BIT(opcode); break;
    case 0x2D: AND(opcode); break;
    case 0x2E: ROL(opcode); break;
    case 0x30: BMI(opcode); break;
    case 0x31: AND(opcode); break;
    case 0x35: AND(opcode); break;
    case 0x36: ROL(opcode); break;
    case 0x38: SEC(opcode); break;
    case 0x39: AND(opcode); break;
    case 0x3D: AND(opcode); break;
    case 0x3E: ROL(opcode); break;
    case 0x40: RTI(opcode); break;
    case 0x41: EOR(opcode); break;
    case 0x45: EOR(opcode); break;
    case 0x46: LSR(opcode); break;
    case 0x48: PHA(opcode); break;
    case 0x49: EOR(opcode); break;
    case 0x4A: LSR(opcode); break;
    case 0x4C: JMP(opcode); break;
    case 0x4D: EOR(opcode); break;
    case 0x4E: LSR(opcode); break;
    case 0x50: BVC(opcode); break;
    case 0x51: EOR(opcode); break;
    case 0x55: EOR(opcode); break;
    case 0x56: LSR(opcode); break;
    case 0x58: CLI(opcode); break;
    case 0x59: EOR(opcode); break;
    case 0x5D: EOR(opcode); break;
    case 0x5E: LSR(opcode); break;
    case 0x60: RTS(opcode); break;
    case 0x61: ADC(opcode); break;
    case 0x65: ADC(opcode); break;
    case 0x66: ROR(opcode); break;
    case 0x68: PLA(opcode); break;
    case 0x69: ADC(opcode); break;
    case 0x6A: ROR(opcode); break;
    case 0x6C: JMP(opcode); break;
    case 0x6D: ADC(opcode); break;
    case 0x6E: ROR(opcode); break;
    case 0x70: BVS(opcode); break;
    case 0x71: ADC(opcode); break;
    case 0x75: ADC(opcode); break;
    case 0x76: ROR(opcode); break;
    case 0x78: SEI(opcode); break;
    case 0x79: ADC(opcode); break;
    case 0x7D: ADC(opcode); break;
    case 0x7E: ROR(opcode); break;
    case 0x81: STA(opcode); break;
    case 0x84: STY(opcode); break;
    case 0x85: STA(opcode); break;
    case 0x86: STX(opcode); break;
    case 0x88: DEY(opcode); break;
    case 0x8A: TXA(opcode); break;
    case 0x8C: STY(opcode); break;
    case 0x8D: STA(opcode); break;
    case 0x8E: STX(opcode); break;
    case 0x90: BCC(opcode); break;
    case 0x91: STA(opcode); break;
    case 0x94: STY(opcode); break;
    case 0x95: STA(opcode); break;
    case 0x96: STX(opcode); break;
    case 0x98: TYA(opcode); break;
    case 0x99: STA(opcode); break;
    case 0x9A: TXS(opcode); break;
    case 0x9D: STA(opcode); break;
    case 0xA0: LDY(opcode); break;
    case 0xA1: LDA(opcode); break;
    case 0xA2: LDX(opcode); break;
    case 0xA4: LDY(opcode); break;
    case 0xA5: LDA(opcode); break;
    case 0xA6: LDX(opcode); break;
    case 0xA8: TAY(opcode); break;
    case 0xA9: LDA(opcode); break;
    case 0xAA: TAX(opcode); break;
    case 0xAC: LDY(opcode); break;
    case 0xAD: LDA(opcode); break;
    case 0xAE: LDX(opcode); break;
    case 0xB0: BCS(opcode); break;
    case 0xB1: LDA(opcode); break;
    case 0xB4: LDY(opcode); break;
    case 0xB5: LDA(opcode); break;
    case 0xB6: LDX(opcode); break;
    case 0xB8: CLV(opcode); break;
    case 0xB9: LDA(opcode); break;
    case 0xBA: TSX(opcode); break;
    case 0xBC: LDY(opcode); break;
    case 0xBD: LDA(opcode); break;
    case 0xBE: LDX(opcode); break;
    case 0xC0: CPY(opcode); break;
    case 0xC1: CMP(opcode); break;
    case 0xC4: CPY(opcode); break;
    case 0xC5: CMP(opcode); break;
    case 0xC6: DEC(opcode); break;
    case 0xC8: INY(opcode); break;
    case 0xC9: CMP(opcode); break;
    case 0xCA: DEX(opcode); break;
    case 0xCC: CPY(opcode); break;
    case 0xCD: CMP(opcode); break;
    case 0xCE: DEC(opcode); break;
    case 0xD0: BNE(opcode); break;
    case 0xD1: CMP(opcode); break;
    case 0xD5: CMP(opcode); break;
    case 0xD6: DEC(opcode); break;
    case 0xD8: CLD(opcode); break;
    case 0xD9: CMP(opcode); break;
    case 0xDD: CMP(opcode); break;
    case 0xDE: DEC(opcode); break;
    case 0xE0: CPX(opcode); break;
    case 0xE1: SBC(opcode); break;
    case 0xE4: CPX(opcode); break;
    case 0xE5: SBC(opcode); break;
    case 0xE6: INC(opcode); break;
    case 0xE8: INX(opcode); break;
    case 0xE9: SBC(opcode); break;
    case 0xEA: NOP(opcode); break;
    case 0xEC: CPX(opcode); break;
    case 0xED: SBC(opcode); break;
    case 0xEE: INC(opcode); break;
    case 0xF0: BEQ(opcode); break;
    case 0xF1: SBC(opcode); break;
    case 0xF5: SBC(opcode); break;
    case 0xF6: INC(opcode); break;
    case 0xF8: SED(opcode); break;
    case 0xF9: SBC(opcode); break;
    case 0xFD: SBC(opcode); break;
    case 0xFE: INC(opcode); break;
    default:
        std::cout << "Unexpected opcode: " << opcode << std::endl;
    }
}