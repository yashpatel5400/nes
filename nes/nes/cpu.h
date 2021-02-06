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
enum class AddressMode {
	Immidiate,
	Zeropage,
	Absolute,
	Indirect
};

class CPU {
public:
	CPU(uint8_t* memory);
	~CPU() = default;

	void step();
    void dump(); // dumps state (just used for debugging purposes)

private: 
	// SR Flags (bit 7 to bit 0) carry different semantics -- functions to disentangle
    bool srN();
    bool srV();
    bool srB();
    bool srD();
    bool srI();
    bool srZ();
    bool srC();

    uint8_t operandAcc();
    uint16_t operandAbs();
    uint16_t operandAbsX();
    uint16_t operandAbsY();
    uint8_t operandImm();
    uint16_t operandInd();
    uint8_t operandIndX();
    uint8_t operandIndY();
    uint8_t operandRelative();
    uint8_t operandZpg();
    uint8_t operandZpgX();
    uint8_t operandZpgX();

    void ADC(uint16_t opcode);
    void AND(uint16_t opcode);
    void ASL(uint16_t opcode);
    void BCC(uint16_t opcode);
    void BCS(uint16_t opcode);
    void BEQ(uint16_t opcode);
    void BIT(uint16_t opcode);
    void BMI(uint16_t opcode);
    void BNE(uint16_t opcode);
    void BPL(uint16_t opcode);
    void BRK(uint16_t opcode);
    void BVC(uint16_t opcode);
    void BVS(uint16_t opcode);
    void CLC(uint16_t opcode);
    void CLD(uint16_t opcode);
    void CLI(uint16_t opcode);
    void CLV(uint16_t opcode);
    void CMP(uint16_t opcode);
    void CPX(uint16_t opcode);
    void CPY(uint16_t opcode);
    void DEC(uint16_t opcode);
    void DEX(uint16_t opcode);
    void DEY(uint16_t opcode);
    void EOR(uint16_t opcode);
    void INC(uint16_t opcode);
    void INX(uint16_t opcode);
    void INY(uint16_t opcode);
    void JMP(uint16_t opcode);
    void JSR(uint16_t opcode);
    void LDA(uint16_t opcode);
    void LDX(uint16_t opcode);
    void LDY(uint16_t opcode);
    void LSR(uint16_t opcode);
    void NOP(uint16_t opcode);
    void ORA(uint16_t opcode);
    void PHA(uint16_t opcode);
    void PHP(uint16_t opcode);
    void PLA(uint16_t opcode);
    void PLP(uint16_t opcode);
    void ROL(uint16_t opcode);
    void ROR(uint16_t opcode);
    void RTI(uint16_t opcode);
    void RTS(uint16_t opcode);
    void SBC(uint16_t opcode);
    void SEC(uint16_t opcode);
    void SED(uint16_t opcode);
    void SEI(uint16_t opcode);
    void STA(uint16_t opcode);
    void STX(uint16_t opcode);
    void STY(uint16_t opcode);
    void TAX(uint16_t opcode);
    void TAY(uint16_t opcode);
    void TSX(uint16_t opcode);
    void TXA(uint16_t opcode);
    void TXS(uint16_t opcode);
    void TYA(uint16_t opcode);

    uint16_t readShort(uint16_t addr);

	uint8_t* memory;

    uint16_t opcode;
	uint16_t rpc; // program counter (16 bit)
	uint8_t rac;  // accumulator (8 bit)
	uint8_t rx;   // X register  (8 bit)
	uint8_t ry;   // Y register  (8 bit)
	uint8_t rsr;  // status register [NV-BDIZC]  (8 bit)
	uint8_t rsp;  // stack pointer   (8 bit)
};