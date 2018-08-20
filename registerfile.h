#pragma once
#include <string>
#include "memory.h"

class registerfile
{
private:
	memory registers = memory(24 * 4);
public:
	registerfile();
	void reset();
	void set(uint32_t r, int32_t val);
	int32_t get(uint32_t r) const;
	void dump(const std::string & hdr) const;
};

