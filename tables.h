#pragma once
#include <string> 
#include <vector> 
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

enum token
{
	KEY_WORD = 0, OPERATOR = 1, SEPARATOR = 2, VARIABLE = 3, CONSTANT = 4, NULL_TOKEN = 5, UNDEFIND = 6
};

struct variable
{
	string name; // имя переменной
	int type; // тип переменной
	string value; // значение переменной
	pair <int, int> local_adress; //локальный адресс переменной в стеке
	variable(string n, int t, string values); // конструктор структуры
	variable();
};

struct triad_index
{
	int cur_table; // тип таблицы
	int index; // порядковый номер элемента таблицы
	int sub_index; // порядковый номер элемента хеш-таблицы с коллизией
	triad_index(int c_t, int i, int s_i = 0);
	triad_index(int c_t, pair <int, int> par_index);
	static vector <triad_index> read_tokens(ifstream file);
	bool operator ==(const triad_index right);
};

class constant_table
{
private:
	vector <string> table;
public:
	bool add_element(string obj);
	int contained_in_table(string obj);
	constant_table();
	string get_name(triad_index el);
};

class variable_table
{
private:
	vector <vector <variable>> objects;
	unsigned int calc_hash(string obj);
	pair <int, int> contained_in_table(variable obj, int hash);
public:
	bool equal_object(variable obj_1, variable obj_2);
	pair <int, int> contained_in_table(variable obj);
	pair <int, int> contained_in_table(string obj_name);
	bool change_value(string value, int hash, int p);
	bool change_value(string value, pair <int, int> ind);
	bool change_type(int type, int hash, int p);
	bool change_type(int type, triad_index tr);
	bool set_local_index(triad_index el, pair <int, int> value);
	int get_type(triad_index tr);
	bool get_value(string& value, int hash, int p);
	bool get_value(string& value, pair <int, int> ind);
	pair <int, int> get_local_index(triad_index el);
	pair <int, int> add_element(variable obj);
	pair <int, int> add_element(string obj);
	int print_table(ofstream f);
	vector <variable> get_all_elems();
	variable_table(int size_table = 5000);
	variable_table(string file_name);
	string get_name(triad_index el);
};

class system_tables
{
private:
	void init();
public:
	constant_table key_words, operators, separators;
	variable_table vars, constants;
	system_tables(int size_tables = 512);
	system_tables(string const_table, string var_table);
	string get_name(triad_index el, int mode = 0);
};

