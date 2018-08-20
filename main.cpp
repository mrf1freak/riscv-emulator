#include <iostream>
#include <string>
#include "cpu_single_hart.h"


int main(int argc, char ** argv) {
    
    if (argc < 2) { std::cout << "Missing file argument" << std::endl; return -1; }

    memory mem = memory(0x120000);
    cpu_single_hart cpu = cpu_single_hart(mem);
    mem.load_file(argv[1]);

    std::string dump_here = "";
    cpu.set_show_instructions(true);
    cpu.run(0);

    return 0;
}