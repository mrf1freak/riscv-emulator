#include <string>
#include "rv32i_decode.h"
#include "registerfile.h"

class rv32i_hart : public rv32i_decode
{
public:
	rv32i_hart(memory& m) : mem(m) { show_instructions = false; show_registers = false; }
	void set_show_instructions(bool b) { show_instructions = b; }
	void set_show_registers(bool b) { show_registers = b; }
	bool is_halted() const { return halt; }
	const std::string& get_halt_reason() const { return halt_reason; }
	uint64_t get_insn_counter() const { return insn_counter; }
	void set_mhartid(int i) { mhartid = i; }
	void tick(const std::string& hdr = "");
	void dump(const std::string& hdr = "") const;
	void reset();

private:
	static constexpr int instruction_width = 35;
	void exec(uint32_t insn, std::ostream*);
	void exec_lui(uint32_t insn, std::ostream*);
	void exec_auipc(uint32_t insn, std::ostream*);
	void exec_jal(uint32_t insn, std::ostream*);
	void exec_jalr(uint32_t insn, std::ostream*);
	
	void exec_bne(uint32_t insn, std::ostream*);
	void exec_blt(uint32_t insn, std::ostream*);
	void exec_bge(uint32_t insn, std::ostream*);
	void exec_bltu(uint32_t insn, std::ostream*);
	void exec_bgeu(uint32_t insn, std::ostream*);
	void exec_beq(uint32_t insn, std::ostream*);
	

	void exec_addi(uint32_t insn, std::ostream* pos);


	void exec_lbu(uint32_t insn, std::ostream* pos);
	void exec_lhu(uint32_t insn, std::ostream* pos);
	void exec_lb(uint32_t insn, std::ostream* pos);
	void exec_lh(uint32_t insn, std::ostream* pos);
	void exec_lw(uint32_t insn, std::ostream* pos);

	void exec_sb(uint32_t insn, std::ostream* pos);
	void exec_sh(uint32_t insn, std::ostream* pos);
	void exec_sw(uint32_t insn, std::ostream* pos);

	void exec_slti(uint32_t insn, std::ostream* pos);
	void exec_sltiu(uint32_t insn, std::ostream* pos);
	void exec_xori(uint32_t insn, std::ostream* pos);
	void exec_ori(uint32_t insn, std::ostream* pos);
	void exec_andi(uint32_t insn, std::ostream* pos);
	void exec_slli(uint32_t insn, std::ostream* pos);
	void exec_srli(uint32_t insn, std::ostream* pos);
	void exec_srai(uint32_t insn, std::ostream* pos);

	void exec_add(uint32_t insn, std::ostream* pos);
	void exec_sub(uint32_t insn, std::ostream* pos);
	void exec_sll(uint32_t insn, std::ostream* pos);
	void exec_slt(uint32_t insn, std::ostream* pos);
	void exec_sltu(uint32_t insn, std::ostream* pos);
	void exec_xor(uint32_t insn, std::ostream* pos);
	void exec_srl(uint32_t insn, std::ostream* pos);
	void exec_sra(uint32_t insn, std::ostream* pos);
	void exec_or(uint32_t insn, std::ostream* pos);
	void exec_and(uint32_t insn, std::ostream* pos);


	void exec_csrrs(uint32_t insn, std::ostream* pos);
	void exec_csrrc(uint32_t insn, std::ostream* pos);
	void exec_csrrw(uint32_t insn, std::ostream* pos);
	void exec_csrrsi(uint32_t insn, std::ostream* pos);
	void exec_csrrci(uint32_t insn, std::ostream* pos);
	void exec_csrrwi(uint32_t insn, std::ostream* pos);


	
	void exec_illegal_insn(uint32_t insn, std::ostream*);
	void exec_ebreak(uint32_t insn, std::ostream*);

	bool halt = { false };
	std::string halt_reason = { "none" };
	
	uint64_t insn_counter = { 0 };
	uint32_t pc = { 0 };
	uint32_t mhartid = { 0 };

protected:
	registerfile regs;
	memory& mem;
	bool show_instructions, show_registers;

};