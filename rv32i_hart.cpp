#include <iostream>
#include <iomanip>
#include "rv32i_hart.h"

void rv32i_hart::tick(const std::string& hdr)
{
	if (halt) return;

	insn_counter++;
	if (show_registers) dump();
	uint32_t insn = mem.get32(pc);
	if (show_instructions) {
		std::cout << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
		exec(insn, &std::cout);
		std::cout << std::endl;

	}
	else {
		exec(insn, nullptr);
	}

}

void rv32i_hart::dump(const std::string& hdr) const
{
	regs.dump(hdr);
	std::cout << hdr << std::setw(3) << "pc" << " " << hex::to_hex32(pc) << std::endl;
}

void rv32i_hart::reset()
{
	pc = 0;
	regs.reset();
	insn_counter = 0;
	halt = false;
	halt_reason = "none";
}

void rv32i_hart::exec_ebreak(uint32_t insn, std::ostream* pos)
{
	if (pos)
	{
	 std::string s = render_ebreak(insn);
	 *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
	 *pos << "// HALT ";
	 }
	halt = true;
	halt_reason = "EBREAK instruction";
}

void rv32i_hart::exec(uint32_t insn, std::ostream* pos)
{
	uint32_t opcode = get_opcode(insn);
	if (insn == insn_ebreak) return exec_ebreak(insn, pos);

	switch (opcode) {
	default: exec_illegal_insn(insn, pos); return;
	case opcode_lui: return exec_lui(insn, pos);
	case opcode_auipc: return exec_auipc(insn, pos);
	case opcode_jal: return exec_jal(insn, pos);
	case opcode_jalr: return exec_jalr(insn, pos);

	case opcode_btype:
		switch (get_funct3(insn))
		{
		default: return exec_illegal_insn(insn, pos);
		case funct3_beq: return exec_beq(insn, pos);
		case funct3_bne: return exec_bne(insn, pos);
		case funct3_blt: return exec_blt(insn, pos);
		case funct3_bge: return exec_bge(insn, pos);
		case funct3_bltu: return exec_bltu(insn, pos);
		case funct3_bgeu: return exec_bgeu(insn, pos);
		}

	case opcode_load_imm:
		switch (get_funct3(insn))
		{
		default: return exec_illegal_insn(insn, pos);
		case funct3_lb: return exec_lb(insn, pos);
		case funct3_lh: return exec_lh(insn, pos);
		case funct3_lw: return exec_lw(insn, pos);
		case funct3_lbu: return exec_lbu(insn, pos);
		case funct3_lhu: return exec_lhu(insn, pos);
		}

	case opcode_stype:
		switch (get_funct3(insn))
		{
		default: return exec_illegal_insn(insn, pos);
		case funct3_sb: return exec_sb(insn, pos);
		case funct3_sh: return exec_sh(insn, pos);
		case funct3_sw: return exec_sw(insn, pos);
		}

	case opcode_alu_imm:
		switch (get_funct3(insn))
		{
		default: return exec_illegal_insn(insn, pos);
		case funct3_add: return exec_addi(insn, pos);
		case funct3_slt: return exec_slti(insn, pos);
		case funct3_sltu: return exec_sltiu(insn, pos);
		case funct3_xor: return exec_xori(insn, pos);
		case funct3_or: return exec_ori(insn, pos);
		case funct3_and: return exec_andi(insn, pos);
		case funct3_sll: return exec_slli(insn, pos);
		case funct3_srx:
			switch (get_funct7(insn))
			{
			default: return exec_illegal_insn(insn, pos);
			case funct7_sra: return exec_srai(insn, pos);
			case funct7_srl: return exec_srli(insn, pos);
			}

			
		}

	case opcode_rtype:
		switch (get_funct3(insn))
		{
		default: return exec_illegal_insn(insn, pos);
		case funct3_add: return exec_add(insn, pos);
		case funct3_and: return exec_and(insn, pos);
		case funct3_or: return exec_or(insn, pos);
		case funct3_sll: return exec_sll(insn, pos);
		case funct3_slt: return exec_slt(insn, pos);
		case funct3_sltu: return exec_sltu(insn, pos);
		case funct3_xor: return exec_xor(insn, pos);
		case funct3_srx:
			switch (get_funct7(insn))
			{
			default: return exec_illegal_insn(insn, pos);
			case funct7_sra: return exec_sra(insn, pos);
			case funct7_srl: return exec_srl(insn, pos);
			}
			
		}

	case opcode_system:
		switch (get_funct3(insn))
		{
		default: return exec_illegal_insn(insn, pos);
		case funct3_csrrw: return exec_csrrs(insn, pos);
		case funct3_csrrs: return exec_csrrs(insn, pos);
		case funct3_csrrc: return exec_csrrs(insn, pos);
		case funct3_csrrwi: return exec_csrrs(insn, pos);
		case funct3_csrrsi: return exec_csrrs(insn, pos);
		case funct3_csrrci: return exec_csrrs(insn, pos);
		}
	}


}

void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn) << 12;
	
	regs.set(rd, imm_u);

	if (pos) {
		std::string s = decode(0, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(imm_u);
	}

	pc += 4;
}

void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn) << 12;

	regs.set(rd, imm_u + pc);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(pc) << " + " << to_hex0x32(imm_u) << " = " << to_hex0x32(imm_u + pc);
	}
	pc += 4;
}

void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_j(insn);

	regs.set(rd, pc + 4);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(pc + 4) << ", pc = " << to_hex0x32(pc) << " + " << to_hex0x32(imm_u) << " = " << to_hex0x32(imm_u);
	}
	pc += imm_u;
}

void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	
	uint32_t rs_value = regs.get(rs);
	regs.set(rd, pc + 4);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(pc + 4) << ", pc = (" << to_hex0x32(imm_u) << " + " << to_hex0x32(rs_value) << ") & " << to_hex0x32(0xfffffffe) << " = " << to_hex0x32((imm_u + rs_value) & 0xfffffffe);
	}
	pc = (imm_u + rs_value) & 0xfffffffe;
}

void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t imm_u = get_imm_b(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2);

	uint32_t pc_increment = rs1_value != rs2_value ? imm_u : 4;
	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// pc += (" << to_hex0x32(rs1_value) << " != " << to_hex0x32(rs2_value) << " ? " << to_hex0x32(imm_u) << " : 4) = " << to_hex0x32(pc_increment + pc);
	}
	pc += pc_increment;
}

void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t imm_u = get_imm_b(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2);

	uint32_t pc_increment = rs1_value < rs2_value ? imm_u : 4;
	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// pc += (" << to_hex0x32(rs1_value) << " < " << to_hex0x32(rs2_value) << " ? " << to_hex0x32(imm_u) << " : 4) = " << to_hex0x32(pc_increment + pc);
	}
	pc += pc_increment;
}

void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t imm_u = get_imm_b(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2);

	int32_t pc_increment = rs1_value >= rs2_value ? imm_u : 4;
	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// pc += (" << to_hex0x32(rs1_value) << " >= " << to_hex0x32(rs2_value) << " ? " << to_hex0x32(imm_u) << " : 4) = " << to_hex0x32(pc_increment + pc);
	}
	pc += pc_increment;
}

void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t imm_u = get_imm_b(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2);

	uint32_t pc_increment = (unsigned)rs1_value < (unsigned)rs2_value ? imm_u : 4;
	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// pc += (" << to_hex0x32(rs1_value) << " <U " << to_hex0x32(rs2_value) << " ? " << to_hex0x32(imm_u) << " : 4) = " << to_hex0x32(pc_increment + pc);
	}
	pc += pc_increment;
}

void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t imm_u = get_imm_b(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2);

	uint32_t pc_increment = (unsigned)rs1_value >= (unsigned)rs2_value ? imm_u : 4;
	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// pc += (" << to_hex0x32(rs1_value) << " >=U " << to_hex0x32(rs2_value) << " ? " << to_hex0x32(imm_u) << " : 4) = " << to_hex0x32(pc_increment + pc);
	}
	pc += pc_increment;
}

void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t imm_u = get_imm_b(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2);

	uint32_t pc_increment = rs1_value == rs2_value ? imm_u : 4;
	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// pc += (" << to_hex0x32(rs1_value) << " == " << to_hex0x32(rs2_value) << " ? " << to_hex0x32(imm_u) << " : 4) = " << to_hex0x32(pc_increment + pc);
	}
	pc += pc_increment;
}

void rv32i_hart::exec_addi(uint32_t insn, std::ostream* pos) {
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);

	uint32_t value = rs1_value + imm_i;
	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " + " << to_hex0x32(imm_i) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos) {
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t addr = rs1_value + imm_u;
	uint32_t data = mem.get8(addr);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = zx(m8(" << to_hex0x32(imm_u) << " + " << to_hex0x32(rs1_value) << ")) = " << to_hex0x32(data);
	}

	pc += 4;
}

void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos) {
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t addr = rs1_value + imm_u;
	uint32_t data = mem.get16(addr);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = zx(m8(" << to_hex0x32(imm_u) << " + " << to_hex0x32(rs1_value) << ")) = " << to_hex0x32(data);
	}

	pc += 4;
}

void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos) {
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t addr = rs1_value + imm_u;
	uint32_t data = mem.get8_sx(addr);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = zx(m8(" << to_hex0x32(imm_u) << " + " << to_hex0x32(rs1_value) << ")) = " << to_hex0x32(data);
	}

	pc += 4;
}

void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos) {
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t addr = rs1_value + imm_u;
	uint32_t data = mem.get16_sx(addr);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = zx(m8(" << to_hex0x32(imm_u) << " + " << to_hex0x32(rs1_value) << ")) = " << to_hex0x32(data);
	}

	pc += 4;
}

void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos) {
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t addr = rs1_value + imm_u;
	uint32_t data = mem.get32(addr);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = zx(m8(" << to_hex0x32(imm_u) << " + " << to_hex0x32(rs1_value) << ")) = " << to_hex0x32(data);
	}

	pc += 4;
}

void rv32i_hart::exec_sb(uint32_t insn, std::ostream* pos) {
	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_s(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2) & 0x000000ff;
	uint32_t addr = rs1_value + imm_u;
	mem.set8(addr, rs2_value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// m8(" << to_hex0x32(rs1_value) << " + " << to_hex0x32(imm_u) << ") = " << to_hex0x32(rs2_value);
	}

	pc += 4;
}

void rv32i_hart::exec_sh(uint32_t insn, std::ostream* pos) {
	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_s(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2) & 0x0000ffff;
	uint32_t addr = rs1_value + imm_u;
	mem.set16(addr, rs2_value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// m8(" << to_hex0x32(rs1_value) << " + " << to_hex0x32(imm_u) << ") = " << to_hex0x32(rs2_value);
	}

	pc += 4;
}

void rv32i_hart::exec_sw(uint32_t insn, std::ostream* pos) {
	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_s(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2);
	uint32_t addr = rs1_value + imm_u;
	mem.set32(addr, rs2_value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// m8(" << to_hex0x32(rs1_value) << " + " << to_hex0x32(imm_u) << ") = " << to_hex0x32(rs2_value);
	}

	pc += 4;
}

void rv32i_hart::exec_slti(uint32_t insn, std::ostream* pos)
{
	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1_value < imm_u ? 1 : 0;

	regs.set(rs2, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rs2 << " = (" << to_hex0x32(rs1_value) << " < " << imm_u << ") ? 1 : 0 = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_sltiu(uint32_t insn, std::ostream* pos)
{
	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	int32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1 < imm_u ? 1 : 0;

	regs.set(rs2, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rs2 << " = (" << to_hex0x32(rs1_value) << " <U " << imm_u << ") ? 1 : 0 = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_xori(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	int32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1_value ^ imm_u;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " ^ " << to_hex0x32(imm_u) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_ori(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	int32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1_value | imm_u;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " | " << to_hex0x32(imm_u) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_andi(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	int32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1_value & imm_u;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " & " << to_hex0x32(imm_u) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_slli(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	int32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1_value << imm_u;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " << " << imm_u << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_srli(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1_value >> imm_u;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " >> " << imm_u << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_srai(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_u = get_imm_i(insn);

	int32_t rs1_value = regs.get(rs1);
	uint32_t value = rs1_value >> imm_u;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " >> " << imm_u << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_add(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2);
	uint32_t value = rs1_value + rs2_value;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " + " << to_hex0x32(rs2_value) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_sll(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2) & 0b11111;
	uint32_t value = rs1_value << rs2_value;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " << " << rs2_value << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_slt(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2) & 0b11111;
	uint32_t value = (rs1_value < rs2_value) ? 1 : 0;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = (" << to_hex0x32(rs1_value) << " < " << rs2_value << ") ? 1 : 0 = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_sltu(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2) & 0b11111;
	uint32_t value = (rs1_value < rs2_value) ? 1 : 0;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = (" << to_hex0x32(rs1_value) << " <U " << rs2_value << ") ? 1 : 0 = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_xor(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2);
	uint32_t value = rs1_value ^ rs2_value;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " ^ " << to_hex0x32(rs2_value) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_srl(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	uint32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2) & 0b11111;
	uint32_t value = rs1_value >> rs2_value;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " >> " << rs2_value << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_sra(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t rs1_value = regs.get(rs1);
	uint32_t rs2_value = regs.get(rs2) & 0b11111;
	uint32_t value = rs1_value >> rs2_value;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " >> " << rs2_value << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_or(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2);
	uint32_t value = rs1_value | rs2_value;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " | " << to_hex0x32(rs2_value) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_and(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rs2_value = regs.get(rs2);
	uint32_t value = rs1_value & rs2_value;

	regs.set(rd, value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << to_hex0x32(rs1_value) << " & " << to_hex0x32(rs2_value) << " = " << to_hex0x32(value);
	}

	pc += 4;
}

void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t imm = get_imm_i(insn);

	int32_t rs1_value = regs.get(rs1);
	int32_t rd_value = regs.get(rd);

	regs.set(rd, rs1_value);

	if (pos) {
		std::string s = decode(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// x" << rd << " = " << rs1_value;
	}

	pc += 4;
}



void rv32i_hart::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
	(void) insn;
	if (pos) {
		std::string s = decode(0, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// ILLEGAL INSTRUCTION ";
	}
	halt = true;
	halt_reason = "Illegal instruction ";

}

