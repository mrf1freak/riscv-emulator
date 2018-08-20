#include "registerfile.h"
#include <iostream>
#include <iomanip>
#include "hex.h"

registerfile::registerfile()
{
	registers = memory(32 * 4);
	for (uint32_t i = 0; i < registers.get_size(); i++) registers.set8(i, 0xf0);
	
	registers.set32(0, 0);
}

void registerfile::reset()
{
	for (uint32_t i = 0; i < registers.get_size(); i++) {
		registers.set8(i, 0);
	}
}

void registerfile::set(uint32_t r, int32_t val)
{
	if(r > 0) registers.set32(r * 4, val);
}

int32_t registerfile::get(uint32_t r) const
{
	return registers.get32(r * 4);
}

void registerfile::dump(const std::string& hdr) const
{
	for (uint32_t i = 0; i < registers.get_size(); i += 4) {
		if (i % 32 == 0) {
			std::string register_name = "x" + std::to_string(i / 4);
			std::cout << hdr << std::setw(3) << register_name;
		}

		std::cout << " " << hex::to_hex32(registers.get32(i));
		if ((i + 20) % 32 == 0) std::cout << " ";
		if ((i + 4) % 32 == 0) std::cout << std::endl;
	}
}
