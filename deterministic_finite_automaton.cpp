#include "deterministic_finite_automation.h"

way::way(int _to, token _token1, token _token2)
{
	to = _to;
	str_push = _token1;
	char_push = _token2;
}

probably_way::probably_way(vector<char> c, way ch_st) : change_state(ch_st), list(c)
{}

probably_way::probably_way(vector<char> c, vector<char> unclude, way ch_st) : change_state(ch_st), list(c)
{
	for (int i = 0; i < unclude.size(); ++i)
	{
		vector<char>::iterator iter = find(list.begin(), list.end(), unclude[i]);
		if (iter != list.end())
		{
			list.erase(iter);
		}
	}
}

int DFA::defind_way(char step)
{
	int default_way = -1;
	for (int i = 0; i < ways[now.st].size(); ++i)
	{
		for (char c : ways[now.st][i].list)
		{
			if (step == c)
			{
				return i;
			}
			if (c == 0)
			{
				default_way = i;
			}
		}
	}
	return default_way;
}

int DFA::next_state(char step)//
{
	int id_way = defind_way(step);
	if (id_way == -1)
	{
		cerr << "(" << cur_y << "," << cur_x << "): Error in " << now.st << " state. Unexpected char: " << step << endl;
		return 1;
	}

	way next_st = ways[now.st][id_way].change_state;

	now.c = step;
	now.st = next_st.to;

	if (now.c != 0 && next_st.char_push == UNDEFIND)
	{
		now.str += now.c;
		now.c = 0;
	}

	switch (next_st.str_push)
	{
	case SEPARATOR:
		if (now.str != " " && now.str != "\n" && now.str != "\t")
		{
			tokens.push_back({ SEPARATOR, tables->separators.contained_in_table(now.str) });
		}
		now.str = "";
		break;
	case OPERATOR:
		tokens.push_back({ OPERATOR, tables->operators.contained_in_table(now.str) });
		now.str = "";
		break;
	case KEY_WORD:
		tokens.push_back({ KEY_WORD, tables->key_words.contained_in_table(now.str) });
		now.str = "";
		break;
	case VARIABLE:
		tokens.push_back({ VARIABLE, tables->vars.add_element(now.str) });
		now.str = "";
		break;
	case CONSTANT:
		if (now.str == "-")
		{
			tokens.push_back({ OPERATOR, tables->operators.contained_in_table(now.str) });
		}
		else
		{
			tokens.push_back({ CONSTANT, tables->constants.add_element(now.str) });
		}
		now.str = "";
		break;
	case NULL_TOKEN:
		now.str = "";
		break;
	default:
		break;
	}

	switch (next_st.char_push)
	{
	case SEPARATOR:
		if (now.c != ' ' && now.c != '\n' && now.c != '\t')
		{
			tokens.push_back({ SEPARATOR, tables->separators.contained_in_table({now.c}) });
		}
		now.c = 0;
		break;
	case OPERATOR:
		tokens.push_back({ OPERATOR, tables->operators.contained_in_table({now.c}) });
		now.c = 0;
		break;
	case KEY_WORD:
		tokens.push_back({ KEY_WORD, tables->key_words.contained_in_table({now.c}) });
		now.c = 0;
		break;
	case VARIABLE:
		tokens.push_back({ VARIABLE, tables->vars.add_element(string{now.c}) });
		now.c = 0;
		break;
	case CONSTANT:
		tokens.push_back({ CONSTANT, tables->constants.add_element(string{now.c}) });
		now.c = 0;
		break;
	case NULL_TOKEN:
		now.c = 0;
		break;
	default:
		break;
	}
}

DFA::DFA(system_tables& sys_tab) :ways(17, vector <probably_way>(0, { {}, {-1} }))
{
	tables = &sys_tab;

	vector <char> digits_with_neg(digits);
	vector <char> digits_with_alphabet(digits);
	digits_with_alphabet.insert(digits_with_alphabet.end(), alphabet.begin(), alphabet.end());
	digits_with_neg.push_back('-');

	ways[0].push_back({ separators, {0, SEPARATOR} });
	ways[0].push_back({ digits_with_neg, {1} });
	ways[0].push_back({ alphabet, {'f', 'i'}, {5} });
	ways[0].push_back({ {'=', '>', '<'}, {2} });
	ways[0].push_back({ {'+', '-', '*'}, {0, OPERATOR} });
	ways[0].push_back({ {'&'}, {3} });
	ways[0].push_back({ {'|'}, {4} });
	ways[0].push_back({ {'/'}, {12} });
	ways[0].push_back({ {'f'}, {6} });
	ways[0].push_back({ {'i'}, {9} });

	ways[1].push_back({ digits, {1} });
	ways[1].push_back({ {'|'}, {4, CONSTANT} });
	ways[1].push_back({ {'&'}, {3, CONSTANT} });
	ways[1].push_back({ separators, {0, CONSTANT, SEPARATOR} });
	ways[1].push_back({ {'=', '>', '<'}, {2, CONSTANT} });
	ways[1].push_back({ {'+', '-', '*'}, {0, CONSTANT, OPERATOR} });
	ways[1].push_back({ {'/'}, {12, CONSTANT} });

	ways[2].push_back({ digits_with_neg, {1, OPERATOR} });
	ways[2].push_back({ alphabet, {'f', 'i'}, {5, OPERATOR} });
	ways[2].push_back({ separators, {0, OPERATOR, SEPARATOR} });
	ways[2].push_back({ {'/'}, {12, OPERATOR} });
	ways[2].push_back({ {'='}, {16} });

	ways[3].push_back({ {'&'}, {0, OPERATOR} });

	ways[4].push_back({ {'|'}, {0, OPERATOR} });

	ways[5].push_back({ digits_with_alphabet, {5} });
	ways[5].push_back({ {'|'}, {4, VARIABLE} });
	ways[5].push_back({ {'&'}, {3, VARIABLE} });
	ways[5].push_back({ separators, {0, VARIABLE, SEPARATOR} });
	ways[5].push_back({ {'=', '>', '<'}, {2, VARIABLE} });
	ways[5].push_back({ {'+', '-', '*'}, {0, VARIABLE, OPERATOR} });
	ways[5].push_back({ {'/'}, {12, VARIABLE} });

	ways[6].push_back({ digits_with_alphabet, {'o'}, {5} });
	ways[6].push_back({ {'|'}, {4, VARIABLE} });
	ways[6].push_back({ {'&'}, {3, VARIABLE} });
	ways[6].push_back({ separators, {0, VARIABLE, SEPARATOR} });
	ways[6].push_back({ {'=', '>', '<'}, {2, VARIABLE} });
	ways[6].push_back({ {'+', '-', '*'}, {0, VARIABLE, OPERATOR} });
	ways[6].push_back({ {'/'}, {12, VARIABLE} });
	ways[6].push_back({ {'o'}, {7} });

	ways[7].push_back({ digits_with_alphabet, {'r'}, {5} });
	ways[7].push_back({ {'|'}, {4, VARIABLE} });
	ways[7].push_back({ {'&'}, {3, VARIABLE} });
	ways[7].push_back({ separators, {0, VARIABLE, SEPARATOR} });
	ways[7].push_back({ {'=', '>', '<'}, {2, VARIABLE} });
	ways[7].push_back({ {'+', '-', '*'}, {0, VARIABLE, OPERATOR} });
	ways[7].push_back({ {'/'}, {12, VARIABLE} });
	ways[7].push_back({ {'r'}, {8} });

	ways[8].push_back({ digits_with_alphabet, {5} });
	ways[8].push_back({ separators, {0, KEY_WORD, SEPARATOR} });
	ways[8].push_back({ {'/'}, {12, KEY_WORD} });

	ways[9].push_back({ digits_with_alphabet, {'n'}, {5} });
	ways[9].push_back({ {'|'}, {4, VARIABLE} });
	ways[9].push_back({ {'&'}, {3, VARIABLE} });
	ways[9].push_back({ separators, {0, VARIABLE, SEPARATOR} });
	ways[9].push_back({ {'=', '>', '<'}, {2, VARIABLE} });
	ways[9].push_back({ {'+', '-', '*'}, {0, VARIABLE, OPERATOR} });
	ways[9].push_back({ {'/'}, {12, VARIABLE} });
	ways[9].push_back({ {'n'}, {10} });

	ways[10].push_back({ digits_with_alphabet, {'t'}, {5} });
	ways[10].push_back({ {'|'}, {4, VARIABLE} });
	ways[10].push_back({ {'&'}, {3, VARIABLE} });
	ways[10].push_back({ separators, {0, VARIABLE, SEPARATOR} });
	ways[10].push_back({ {'=', '>', '<'}, {2, VARIABLE} });
	ways[10].push_back({ {'+', '-', '*'}, {0, VARIABLE, OPERATOR} });
	ways[10].push_back({ {'/'}, {12, VARIABLE} });
	ways[10].push_back({ {'t'}, {11} });

	ways[11].push_back({ digits_with_alphabet, {5} });
	ways[11].push_back({ separators, {0, KEY_WORD, SEPARATOR} });
	ways[11].push_back({ {'/'}, {12, KEY_WORD} });

	ways[12].push_back({ {'/'}, {13} });
	ways[12].push_back({ {'*'}, {14} });

	ways[13].push_back({ {'\n'}, {0, NULL_TOKEN, NULL_TOKEN} });
	ways[13].push_back({ {0}, {13} });

	ways[14].push_back({ {'*'}, {15} });
	ways[14].push_back({ {0}, {14} });

	ways[15].push_back({ {'/'}, {0, NULL_TOKEN, NULL_TOKEN} });
	ways[15].push_back({ {'*'}, {15} });
	ways[15].push_back({ {0}, {14} });

	ways[16].push_back({ digits_with_neg, {1, OPERATOR} });
	ways[16].push_back({ alphabet, {'f', 'i'}, {5, OPERATOR} });
	ways[16].push_back({ separators, {0, OPERATOR, SEPARATOR} });
	ways[16].push_back({ {'/'}, {12, OPERATOR} });
}

int DFA::process(ifstream file)
{
	now.st = 0;
	now.c = 0;
	now.str = "";

	cur_x = 1;
	cur_y = 1;

	tokens.clear();

	char buff;
	while (file.get(buff))
	{
		next_state(buff);
		if (buff == '\n')
		{
			++cur_y;
			cur_x = 1;
		}
		else
		{
			++cur_x;
		}
	}
	next_state('\n');
	if (now.st != 0)
	{
		cerr << "Unexpected end-of-file found" << endl;
		return 1;
	}
	return 0;
}

void DFA::print_list(ofstream fout)
{
	for (int i = 0; i < tokens.size(); ++i)
	{
		fout << tokens[i].cur_table << " " << tokens[i].index << " " << tokens[i].sub_index << "\n";
	}

	tables->constants.print_table(ofstream("constant_table.txt"));
	tables->vars.print_table(ofstream("variable_table.txt"));
}
