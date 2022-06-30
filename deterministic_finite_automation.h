#pragma once
#include <string>
#include <vector>
#include "tables.h"
using namespace std;

const vector <char> alphabet = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' }; //символы
const vector <char> digits = { '0','1','2','3','4','5','6','7','8','9' }; //цифры
const vector <char> separators = { ',',';','{','}','(',')',' ', '\n', '\t' };


struct state //(текущее состояние, аккумулятор строки, символ по которому перешли)
{
	int st;
	string str;
	char c;
};

struct way //структура (куда_идём, (необязательно)запись токена, (необязательно)запись второйго токена как символа)
{
	int to;
	token str_push;
	token char_push;
	way(int _to, token _token1 = UNDEFIND, token _token2 = UNDEFIND);
};

struct probably_way
{
	vector <char> list;
	way change_state;
	probably_way(vector <char> c, way ch_st);
	probably_way(vector <char> c, vector <char> unclude, way ch_st);
};

class DFA
{
private:
	int cur_x, cur_y;
	state now;
	vector <vector <probably_way>> ways;//ways[состояние]=[...возможные следующие состояния]
	system_tables* tables;
	int defind_way(char step);
	int next_state(char step);
public:
	vector <triad_index> tokens;
	DFA(system_tables& sys_tab);
	int process(ifstream file);
	void print_list(ofstream fout);
};
