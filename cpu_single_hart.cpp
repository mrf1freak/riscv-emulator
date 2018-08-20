#include <iostream>
#include "cpu_single_hart.h"

void cpu_single_hart::run(uint64_t exec_limit)
{
	regs.set(2, mem.get_size());


	while (!is_halted() && (exec_limit == 0 || get_insn_counter() < exec_limit)) {
		tick();
	}
	
	std::cout << "Execution terminated. Reason: " << get_halt_reason() << std::endl;
	std::cout << get_insn_counter() << " instructions executed" << std::endl;
}

