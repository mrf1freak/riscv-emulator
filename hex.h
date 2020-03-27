//****************************************************************************
//
// CSCI 463 Assignment 3
//
// Author: Sheeza Saleem
//
// Memory Simulator
//
//****************************************************************************

#include <string>

class hex
{
public:
	static std::string to_hex8(uint8_t i);
	static std::string to_hex32(uint32_t i);
	static std::string to_hex0x32(uint32_t i);
	static std::string to_hex0x20(uint32_t i);
	static std::string to_hex0x12(uint32_t i);

};

