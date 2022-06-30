#pragma once
#include <sstream>
#include <stack>
#include "tables.h"

static class translator
{
private:
	static int oper_ptr;
	static string get_assembler_var(system_tables& tables, triad_index el, string reg, int& op_ptr);
	static string init_data_memory(system_tables& tables, const vector<vector<triad_index>> opz);
	static string init_main_process(system_tables& tables, const vector<vector<triad_index>> opz);
public:
	static string get_asm_code(system_tables& tables, const vector<vector<triad_index>> opz);
};

