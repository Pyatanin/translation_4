#include <string> 
#include <vector> 
#include <iostream> 
#include <fstream>
#include <algorithm>
#include "tables.h"
#include "deterministic_finite_automation.h"
#include "syntax_analyzer.h"
#include "translator.h"
using namespace std;

int main()
{
	FILE* err;
	freopen_s(&err, "stderr.txt", "w", stderr);
	system_tables T(512);
	DFA F(T);
	F.process(ifstream("file.txt"));
	syntax_analyzer sy(ifstream("table_parse.csv"));
	T.constants.print_table(ofstream("constant_table.txt"));
	auto k = sy.create_opz(T, F.tokens);
	T.vars.print_table(ofstream("var_table.txt"));
	sy.print_reverse_pols(ofstream("rpols.txt"), T, k);
	ofstream out("source.asm");
	out << translator::get_asm_code(T, k);
}
