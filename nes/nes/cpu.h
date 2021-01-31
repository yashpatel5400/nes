/************************************************************************************

Filename    :   cpu.h
Content     :   Core CPU emulation (header)
Authors     :   Yash Patel

8 - bit 6502 CPU running at 1.79 MHZ.It has 3 general purpose register A / X / Y, and
3 special register P(status) / SP(stack pointer) / PC(program counter, or instruction
	pointer), all of them being 8 - bit except PC which is 16 - bit.

*************************************************************************************/

#include <stdint.h>

// reference 6502 documentation: https://www.masswerk.at/6502/6502_instruction_set.html#PLP
class CPU {
public:
	CPU(uint8_t* memory);
	~CPU() = default;

	void step();

private: 
	// SR Flags (bit 7 to bit 0) carry different semantics -- functions to disentangle
	bool flagNegative();
	bool flagOverflow();
	bool flagignored();
	bool flagBreak();
	bool flagDecimal();
	bool flagInterrupt();
	bool flagZero();
	bool flagCarry();

	void ADC();
	void AND();
	void ASL();
	void BCC();
	void BCS();
	void BEQ();
	void BIT();
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void CPX();
	void CPY();
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void INC();
	void INX();
	void INY();
	void JMP();
	void JSR();
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void ROL();
	void ROR();
	void RTI();
	void RTS();
	void SBC();
	void SEC();
	void SED();
	void SEI();
	void STA();
	void STX();
	void STY();
	void TAX();
	void TAY();
	void TSX();
	void TXA();
	void TXS();
	void TYA();

	uint8_t* memory;

	uint16_t rpc; // program counter (16 bit)
	uint8_t rac;  // accumulator (8 bit)
	uint8_t rx;   // X register  (8 bit)
	uint8_t ry;   // Y register  (8 bit)
	uint8_t rsr;  // status register [NV-BDIZC]  (8 bit)
	uint8_t rsp;  // stack pointer   (8 bit)
};