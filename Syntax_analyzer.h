#pragma once
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>
#include <unordered_map>
#include "tables.h"
using namespace std;

struct element_parser
{
	vector <string> tokens;
	int jmp;
	int accept;
	int st;
	int ret;
	int err;
};

class syntax_analyzer
{
private:
	vector <vector <int>> road_map;
	unordered_map <string, int> dictionary;
	vector <element_parser> table_parser;
	stack <int> st_int;//состояние
	int now_state; // текущее состояние
	bool contained(string token_name); // проверка в множестве токенов, на текущем состоянии
	vector <triad_index> get_opz(system_tables& tables, const vector <triad_index>& tokens, int pos);

	int token_analyzer(system_tables& tables, const vector <triad_index>& tokens);
	int field_selection(system_tables& tables, vector <triad_index>& tokens);

	bool post_procces(system_tables& tables, const vector <triad_index>& tokens);

public:
	syntax_analyzer(ifstream table_file);
	vector <vector <triad_index>> create_opz(system_tables& tables, vector <triad_index>& tokens);
	void print_reverse_pols(ofstream out, system_tables& tables, vector <vector <triad_index>> pols);
};
