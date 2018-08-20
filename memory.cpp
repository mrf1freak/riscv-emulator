//****************************************************************************
//
// CSCI 463 Assignment 3
//
// Author: Sheeza Saleem
//
// Memory Simulator
//
//****************************************************************************

#include "memory.h"
#include "hex.h"
#include <vector>
#include <string>
#include <iostream>

memory::memory(uint32_t siz)
{
	siz = (siz + 15) & 0xfffffff0;
	mem = std::vector<uint8_t>(siz);
	for (unsigned int i = 0; i < mem.size(); i++) {
		mem[i] = 0xA5;
	}
}

memory::~memory()
{
}

bool memory::check_illegal(uint32_t i) const
{
	bool illegal = i >= mem.size();

	if (illegal) {
		std::cout << "WARNING: Address out of range: " << hex::to_hex0x32(i) << std::endl;
	}

	return illegal;
}

uint32_t memory::get_size() const
{
	return mem.size();
}

uint8_t memory::get8(uint32_t addr) const
{
	if (check_illegal(addr)) return 0x0;
	return mem[addr];
}

uint16_t memory::get16(uint32_t addr) const
{
	uint16_t data_r = get8(addr);
	uint16_t data_l = get8(addr + 1) << 8;
	
	return data_l | data_r;
}

uint32_t memory::get32(uint32_t addr) const
{
	uint32_t data_r = get16(addr);
	uint32_t data_l = get16(addr + 2) << 16;
	return data_l | data_r;
}

int32_t memory::get8_sx(uint32_t addr) const
{
	int32_t data = get8(addr);
	int32_t sign = data >> 7;
	if (sign == 1) data = data | 0xFFFFFF00;
	return data;
}

int32_t memory::get16_sx(uint32_t addr) const
{
	int32_t data = get16(addr);
	int32_t sign = data >> 15;
	if (sign == 1) data = data | 0xFFFF0000;
	return data;
}

int32_t memory::get32_sx(uint32_t addr) const
{
	return get32(addr);
}

void memory::set8(uint32_t addr, uint8_t val)
{
	if (!check_illegal(addr)) mem[addr] = val;
}

void memory::set16(uint32_t addr, uint16_t val)
{
	set8(addr + 1, val >> 8);
	set8(addr, val);
}

void memory::set32(uint32_t addr, uint32_t val)
{
	set16(addr + 2, val >> 16);
	set16(addr, val);
}

void memory::dump() const
{
	for (unsigned int i = 0; i < mem.size() / 16; i++) {
		std::cout << hex::to_hex32(i * 16) << ": ";
		for (int j = 0; j < 16; j++) {
			std::cout << hex::to_hex8(get8(i * 16 + j)) << " ";
			if (j == 7) std::cout << " ";
		}

		std::cout << "*";
		for (int j = 0; j < 16; j++) {
			uint8_t ch = get8(i * 16 + j);
			ch = isprint(ch) ? ch : '.';
			std::cout << ch;
		}
		std::cout << "*" << std::endl;

	}
}

bool memory::load_file(const std::string &fname)
{
	std::ifstream infile(fname, std::ios::in | std::ios::binary);
	if (!infile.is_open()) {
		std::cout << "Can't open file '" << fname << "' for reading" << std::endl;
		return false;
	}

	uint8_t i;
	infile >> std::noskipws;
	for (uint32_t addr = 0; infile >> i; ++addr)
	{
		if (check_illegal(addr)) {
			std::cout << "Program too big" << std::endl;
			infile.close();
			return false;
		}

		mem[addr] = i;
	}
	return true;
	
}

