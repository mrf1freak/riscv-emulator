#include "rv32i_decode.h"
#include<iomanip>
#include<sstream>
#include<string>


std::string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
	if (insn == insn_ebreak) return render_ebreak(insn);
	if (insn == insn_ecall) return render_ecall(insn);
	switch (get_opcode(insn))
	{
	default: return render_illegal_insn(insn);
	case opcode_lui: return render_lui(insn);
	case opcode_auipc: return render_auipc(insn);
	case opcode_jal: return render_jal(addr, insn);
	case opcode_jalr: return render_jalr(insn);

	case opcode_btype:
		switch (get_funct3(insn))
		{
		default: return render_illegal_insn(insn);
		case funct3_beq: return render_btype(addr, insn, "beq");
		case funct3_bne: return render_btype(addr, insn, "bne");
		case funct3_blt: return render_btype(addr, insn, "blt");
		case funct3_bge: return render_btype(addr, insn, "bge");
		case funct3_bltu: return render_btype(addr, insn, "bltu");
		case funct3_bgeu: return render_btype(addr, insn, "bgeu");
		}

	case opcode_load_imm:
		switch (get_funct3(insn))
		{
		default: return render_illegal_insn(insn);
		case funct3_lb: return render_itype_load(insn, "lb");
		case funct3_lh: return render_itype_load(insn, "lh");
		case funct3_lw: return render_itype_load(insn, "lw");
		case funct3_lbu: return render_itype_load(insn, "lbu");
		case funct3_lhu: return render_itype_load(insn, "lhu");
		}

	case opcode_stype:
		switch (get_funct3(insn))
		{
		default: return render_illegal_insn(insn);
		case funct3_sb: return render_stype(insn, "sb");
		case funct3_sh: return render_stype(insn, "sh");
		case funct3_sw: return render_stype(insn, "sw");
		}

	case opcode_alu_imm:
		switch (get_funct3(insn))
		{
		default: return render_illegal_insn(insn);
		case funct3_add: return render_itype_alu(insn, "addi", get_imm_i(insn));
		case funct3_slt: return render_itype_alu(insn, "slti", get_imm_i(insn));
		case funct3_sltu: return render_itype_alu(insn, "sltiu", get_imm_i(insn));
		case funct3_xor: return render_itype_alu(insn, "xori", get_imm_i(insn));
		case funct3_or: return render_itype_alu(insn, "ori", get_imm_i(insn));
		case funct3_and: return render_itype_alu(insn, "andi", get_imm_i(insn));
		case funct3_sll: return render_itype_alu(insn, "slli", get_imm_i(insn));
		case funct3_srx:
			switch (get_funct7(insn))
			{
			default: return render_illegal_insn(insn);
			case funct7_sra: return render_itype_alu(insn, "srai", get_imm_i(insn) % XLEN);
			case funct7_srl: return render_itype_alu(insn, "srli", get_imm_i(insn) % XLEN);
			}
		

		}

	case opcode_rtype:
		switch (get_funct3(insn))
		{
		default: return render_illegal_insn(insn);
		case funct3_add: return render_rtype(insn, "add");
		case funct3_and: return render_rtype(insn, "and");
		case funct3_or: return render_rtype(insn, "or");
		case funct3_sll: return render_rtype(insn, "sll");
		case funct3_slt: return render_rtype(insn, "slt");
		case funct3_sltu: return render_rtype(insn, "sltu");
		case funct3_xor: return render_rtype(insn, "xor");
		case funct3_srx:
			switch (get_funct7(insn))
			{
			default: return render_illegal_insn(insn);
			case funct7_sra: return render_rtype(insn, "sra");
			case funct7_srl: return render_rtype(insn, "srl");
			}

		}

	case opcode_system:
		switch (get_funct3(insn))
		{
		default: return render_illegal_insn(insn);
		case funct3_csrrw: return render_csrrx(insn, "csrrw");
		case funct3_csrrs: return render_csrrx(insn, "csrrs");
		case funct3_csrrc: return render_csrrx(insn, "csrrc");
		case funct3_csrrwi: return render_csrrxi(insn, "csrrwi");
		case funct3_csrrsi: return render_csrrxi(insn, "csrrsi");
		case funct3_csrrci: return render_csrrxi(insn, "csrrci");
		}
	}

	
	
}

// get opcode
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
	return (insn & 0x0000007f);
}

// get rd
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
	return (insn << 20) >> 27;
}

// get funct3
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
	return insn << 17 >> 29;
}

// get rs1
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
	return insn << 12 >> 27;
}

// get rs2
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
	return insn << 7 >> 27;
}

// get funct7
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
	return insn >> 25;
}

// get imm
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
	insn >>= 20;
	if (insn & 0x800) insn |= 0xFFFFF000;
	return insn;
}

// get imm u
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
	return insn >> 12;
}

// get imm b
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
	uint32_t rightmost = insn >> 8 << 1 & 0b11110;
	uint32_t right = insn << 1 >> 21 & 0b11111100000;
	uint32_t single_bit = insn << 4 & (0b1 << 11);
	uint32_t left_bit = (insn & (0b1 << 31)) >> 19;
	uint32_t complete = rightmost | right | single_bit | left_bit;

	if (complete >> 12) {
		complete |= 0xFFFFF000;
	}

	return complete;
}

// get imm s
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
	uint32_t right = insn >> 7 & (0xFFFFFFFF >> 27);
	uint32_t left = insn >> 25;
	uint32_t complete = (left << 5) | right;

	if (0x800 & complete) {
		complete |= 0xFFFFF000;
	}

	return complete;
}

// get imm j
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
	uint32_t right = insn >> 21 << 1;
	uint32_t center = insn & 0x000FF000;
	uint32_t complete = center | right;
	if (complete & 0x00080000) {
		complete |= 0xFFF00000;
	}
	
	uint32_t eleventh_bit = (insn >> 20) & 0x1;
	complete |= eleventh_bit << 11;

	return complete;
}

// render illegal instruction
std::string rv32i_decode::render_illegal_insn(uint32_t insn)
{
	return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

// render lui
std::string rv32i_decode::render_lui(uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);
	std::ostringstream os;
	os << render_mnemonic("lui") << render_reg(rd) << ","
		<< hex::to_hex0x20(imm_u);
	return os.str();

}

// render auipc
std::string rv32i_decode::render_auipc(uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);
	std::ostringstream os;
	os << render_mnemonic("auipc") << render_reg(rd) << ","
		<< hex::to_hex0x20(imm_u);
	return os.str();
}

// render jal
std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_j(insn);
	std::ostringstream os;
	os << render_mnemonic("jal") << render_reg(rd) << ","
		<< hex::to_hex0x32(imm_u);
	return os.str();
}

// render jalr
std::string rv32i_decode::render_jalr(uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);
	std::ostringstream os;
	os << render_mnemonic("jalr") << render_reg(rd) << ","
		<< imm_u << "(" << render_reg(rs1) << ")";
	return os.str();
}

// render btype
std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char* mnemonic)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t imm_u = get_imm_b(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rs1) << ","
		<< render_reg(rs2) << "," << to_hex0x32(imm_u + addr);
	return os.str();
}

// render itype
std::string rv32i_decode::render_itype_load(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << ","
		<< imm_u << "(" << render_reg(rs1) << ")";
	return os.str();
}

// render stype
std::string rv32i_decode::render_stype(uint32_t insn, const char* mnemonic)
{
	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_s(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rs2) << ","
		<< imm_u << "(" << render_reg(rs1) << ")";
	return os.str();
}

// render itype alu
std::string rv32i_decode::render_itype_alu(uint32_t insn, const char* mnemonic, int32_t imm_i)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_reg(rs1) << ","
		<< imm_i;
	return os.str();
}

// render rtype
std::string rv32i_decode::render_rtype(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << "," << render_reg(rs1) << ","
		<< render_reg(rs2);
	return os.str();
}

// render ecall
std::string rv32i_decode::render_ecall(uint32_t insn)
{
	return "ecall";
}

// render ebreak
std::string rv32i_decode::render_ebreak(uint32_t insn)
{
	return "ebreak";
}

// render csrrx
std::string rv32i_decode::render_csrrx(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t imm = get_imm_i(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << "," << to_hex0x12(imm) << ","
		<< render_reg(rs1);
	return os.str();
}

// render csrrxi
std::string rv32i_decode::render_csrrxi(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t imm = get_imm_i(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << "," << to_hex0x12(imm) << ","
		<< rs1;
	return os.str();
}

// render reg
std::string rv32i_decode::render_reg(int r)
{
	std::ostringstream os;
	os << "x" << r;
	return os.str();
}

// render mnemonic
std::string rv32i_decode::render_mnemonic(const std::string& m)
{
	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << m;
	return os.str();
}
