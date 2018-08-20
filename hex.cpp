//****************************************************************************
//
// CSCI 463 Assignment 3
//
// Author: Sheeza Saleem
//
// Memory Simulator
//
//****************************************************************************

#include "hex.h"
#include<iomanip>
#include<sstream>
#include<string>

std::string hex::to_hex8(uint8_t i)
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
	return os.str();
}

std::string hex::to_hex32(uint32_t i)
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(8) << i;
	return os.str();
}

std::string hex::to_hex0x32(uint32_t i)
{
	std::ostringstream os;
	os << "0x" << to_hex32(i);
	return os.str();
}

std::string hex::to_hex0x20(uint32_t i)
{
	std::ostringstream os;

	i &= 0xFFFFF;
	os << std::hex << "0x" << i;
	return os.str();
}

std::string hex::to_hex0x12(uint32_t i)
{
	std::ostringstream os;

	i &= 0xFFF;
	os << std::hex << "0x" << std::setw(3) << std::setfill('0') << i;
	return os.str();
}
