/************************************************************************************

Filename    :   CPU.cpp
Content     :   Core CPU emulation
Authors     :   Yash Patel

8 - bit 6502 CPU running at 1.79 MHZ.It has 3 general purpose register A / X / Y, and
3 special register P(status) / SP(stack pointer) / PC(program counter, or instruction
	pointer), all of them being 8 - bit except PC which is 16 - bit.

*************************************************************************************/

#include "CPU.h"

#include <stdexcept>

CPU::CPU(uint8_t* memory) : memory(memory) {
}

bool CPU::flagNegative() {
	return bool(rsr & 0b10000000);
}

bool CPU::flagOverflow() {
	return bool(rsr & 0b01000000);
}

bool CPU::flagignored() {
	return bool(rsr & 0b00100000);
}

bool CPU::flagBreak() {
	return bool(rsr & 0b00010000);
}

bool CPU::flagDecimal() {
	return bool(rsr & 0b00001000);
}

bool CPU::flagInterrupt() {
	return bool(rsr & 0b00000100);
}

bool CPU::flagZero() {
	return bool(rsr & 0b00000010);
}

bool CPU::flagCarry() {
	return bool(rsr & 0b00000001);
}


void CPU::ADC() { // add with carry
}

void CPU::AND() { // and (with accumulator)
}

void CPU::ASL() { // arithmetic shift left
}

void CPU::BCC() { // branch on carry clear
}

void CPU::BCS() { // branch on carry set
}

void CPU::BEQ() { // branch on equal (zero set)
}

void CPU::BIT() { // bit test
}

void CPU::BMI() { // branch on minus (negative set)
}

void CPU::BNE() { // branch on not equal (zero clear)
}

void CPU::BPL() { // branch on plus (negative clear)
}

void CPU::BRK() { // break / interrupt
}

void CPU::BVC() { // branch on overflow clear
}

void CPU::BVS() { // branch on overflow set
}

void CPU::CLC() { // clear carry
}

void CPU::CLD() { // clear decimal
}

void CPU::CLI() { // clear interrupt disable
}

void CPU::CLV() { // clear overflow
}

void CPU::CMP() { // compare (with accumulator)
}

void CPU::CPX() { // compare with X
}

void CPU::CPY() { // compare with Y
}

void CPU::DEC() { // decrement
}

void CPU::DEX() { // decrement X
}

void CPU::DEY() { // decrement Y
}

void CPU::EOR() { // exclusive or (with accumulator)
}

void CPU::INC() { // increment
}

void CPU::INX() { // increment X
}

void CPU::INY() { // increment Y
}

void CPU::JMP() { // jump
}

void CPU::JSR() { // jump subroutine
}

void CPU::LDA() { // load accumulator
}

void CPU::LDX() { // load X
}

void CPU::LDY() { // load Y
}

void CPU::LSR() { // logical shift right
}

void CPU::NOP() { // no operation
}

void CPU::ORA() { // or with accumulator
}

void CPU::PHA() { // push accumulator
}

void CPU::PHP() { // push processor status (SR)
}

void CPU::PLA() { // pull accumulator
}

void CPU::PLP() { // pull processor status (SR)
}

void CPU::ROL() { // rotate left
}

void CPU::ROR() { // rotate right
}

void CPU::RTI() { // return from interrupt
}

void CPU::RTS() { // return from subroutine
}

void CPU::SBC() { // subtract with carry
}

void CPU::SEC() { // set carry
}

void CPU::SED() { // set decimal
}

void CPU::SEI() { // set interrupt disable
}

void CPU::STA() { // store accumulator
}

void CPU::STX() { // store X
}

void CPU::STY() { // store Y
}

void CPU::TAX() { // transfer accumulator to X
}

void CPU::TAY() { // transfer accumulator to Y
}

void CPU::TSX() { // transfer stack pointer to X
}

void CPU::TXA() { // transfer X to accumulator
}

void CPU::TXS() { // transfer X to stack pointer
}

void CPU::TYA() { // transfer Y to accumulator
}

void CPU::step() {
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