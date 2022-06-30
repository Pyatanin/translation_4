#include "syntax_analyzer.h"

bool syntax_analyzer::contained(string token_name)
{
	for (string token : table_parser[now_state].tokens)
	{
		if (token_name == token)
		{
			return true;
		}
	}
	return false;
}

syntax_analyzer::syntax_analyzer(ifstream table_file)
{
	string element;
	while (!table_file.eof())
	{
		element_parser el;
		string line;
		getline(table_file, line);
		stringstream linestream(line);
		for (int i = 0; i < 7; ++i)
		{
			getline(linestream, element, ';');
			if (i == 1)
			{
				stringstream tokens(element);
				string token;
				while (tokens >> token)
				{
					if (token == "comma")
					{
						token = ",";
					}
					else if (token == "semicolon")
					{
						token = ";";
					}
					el.tokens.push_back(token);
				}
			}
			else if (i == 2)
			{
				el.jmp = atoi(element.c_str());
			}
			else if (i == 3)
			{
				el.accept = atoi(element.c_str());
			}
			else if (i == 4)
			{
				el.st = atoi(element.c_str());
			}
			else if (i == 5)
			{
				el.ret = atoi(element.c_str());
			}
			else if (i == 6)
			{
				el.err = atoi(element.c_str());
			}
		}
		table_parser.push_back(el);
	}

	road_map = {
		{4,1,1,1,1,1,1,1,1,1,1,1,1,1,4},
		{2,2,1,1,1,1,1,1,1,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,1,1,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,1,1,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,1,1,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,1,1,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,1,1,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,2,1,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,2,2,1,1,1,1,1,2},
		{2,2,2,2,2,2,2,2,2,2,2,1,1,1,2},
		{2,2,2,2,2,2,2,2,2,2,2,1,1,1,2},
		{2,2,2,2,2,2,2,2,2,2,2,2,2,1,2},
		{2,2,2,2,2,2,2,2,2,2,2,2,2,1,2},
		{4,1,1,1,1,1,1,1,1,1,1,1,1,1,3}
	};

	dictionary = {
		{",",	0},
		{";",	0},
		{"=",	1},
		{">",	2},
		{"<",	3},
		{">=",	4},
		{"<=",	5},
		{"==",	6},
		{"||",	7},
		{"&&",	8},
		{"+",	9},
		{"-",	10},
		{"*",	11},
		{"/",	12},
		{"(",	13},
		{")",	14}
	};

}

int syntax_analyzer::token_analyzer(system_tables& tables, const vector <triad_index>& tokens)
{
	now_state = 0; //начальное состояние
	st_int.push(-1);

	int vs = 0;
	while (vs < tokens.size())
	{
		if (contained(tables.get_name(tokens[vs])))
		{
			if (table_parser[now_state].accept)
			{
				++vs;
			}
			if (table_parser[now_state].st)
			{
				st_int.push(now_state + 1);
			}
			if (table_parser[now_state].jmp >= 0)
			{
				now_state = table_parser[now_state].jmp;
			}
			else if (table_parser[now_state].ret)
			{
				now_state = st_int.top();
				st_int.pop();
			}
		}
		else
		{
			if (table_parser[now_state].err)
			{
				cerr << "Unexpected token: " << tables.get_name(tokens[vs]) << " (" << tokens[vs].cur_table << ", " << tokens[vs].index << ", " << tokens[vs].sub_index << ")\n";
				return -1;
			}
			else
			{
				++now_state;
			}
		}
	}
	if (st_int.size() > 0)
	{
		cerr << "Unexpected end-of-file found\n";
	}

	return 0;
}

int syntax_analyzer::field_selection(system_tables& tables, vector<triad_index>& tokens)
{
	stack <int> state, stack_display;
	int status = 0, pred_status = 0;
	int now_state = -1;
	int next_state = 0;
	int round_brackets_balance = 0;
	int curly_brackets_balance = 0;
	int display = 0, shift = 0;
	bool flag_for = false;
	for (int i = 0; i < tokens.size(); ++i)
	{
		string token_name = tables.get_name(tokens[i]);
		if (token_name == "{" || token_name == "for")
		{
			if (!flag_for)
			{
				state.push(now_state);
				stack_display.push(display);
			}
			if (token_name == "for")
			{
				round_brackets_balance = 0;
				flag_for = true;
			}
			display = display + shift;
			shift = 0;
			now_state = next_state;
			++next_state;
		}
		else if (token_name == "}")
		{
			shift = display - stack_display.top();
			display = stack_display.top();
			stack_display.pop();
			now_state = state.top();
			state.pop();
		}
		if (flag_for)
		{
			if (token_name == "(")
			{
				++round_brackets_balance;
			}
			else if (token_name == ")")
			{
				--round_brackets_balance;
			}
			else if (round_brackets_balance == 0 && (token_name == ";" || token_name == "{"))
			{
				flag_for = false;
				if (token_name == ";")
				{
					shift = display - stack_display.top();
					display = stack_display.top();
					stack_display.pop();
					now_state = state.top();
					state.pop();
				}
			}
		}

		if (token_name == "int")
		{
			pred_status = 1;
			status = 1;
		}
		if (token_name == ";" || token_name == "main")
		{
			status = 0;
			pred_status = 0;
		}
		if (token_name == "=")
		{
			status = 2;
		}
		if (token_name == ",")
		{
			status = pred_status;
		}

		if (status == 1 && token_name == "var" && now_state == 0)
		{
			tables.vars.set_local_index(tokens[i], { 0, -1 });
		}
		else if (status == 1 && token_name == "var" && now_state > 0)
		{
			stringstream local_var;
			local_var << "_" << now_state << tables.vars.get_name(tokens[i]);
			triad_index new_token = { VARIABLE, tables.vars.add_element(local_var.str()) };
			tables.vars.set_local_index(new_token, { display, shift });
			++shift;
			triad_index old_token = tokens[i];
			int j = i;
			if (flag_for)
			{
				--j;
				int local_round_brackets_balance = round_brackets_balance;
				string local_token_name;
				do
				{
					++j;
					local_token_name = tables.get_name(tokens[j]);
					if (tokens[j] == old_token)
					{
						tokens[j] = new_token;
					}
					if (local_token_name == "(")
					{
						++local_round_brackets_balance;
					}
					else if (local_token_name == ")")
					{
						--local_round_brackets_balance;
					}
				} while (local_round_brackets_balance || local_token_name != ";" && local_token_name != "{");
				if (local_token_name == "{")
				{
					curly_brackets_balance = 1;
					++j;
				}
			}
			if (!flag_for)
			{
				curly_brackets_balance = 1;
			}
			for (; curly_brackets_balance && j < tokens.size(); ++j)
			{
				if (tokens[j] == old_token)
				{
					tokens[j] = new_token;
				}
				else
				{
					string local_token_name = tables.get_name(tokens[j]);
					if (local_token_name == "{")
					{
						++curly_brackets_balance;
					}
					else if (local_token_name == "}")
					{
						--curly_brackets_balance;
					}
				}
			}
		}
	}
	return 0;
}
vector<triad_index> syntax_analyzer::get_opz(system_tables& tables, const vector<triad_index>& tokens, int pos)
{
	vector <triad_index> california;
	stack <triad_index> texas;
	string name;
	texas.push({ SEPARATOR, 3, 0 });
	bool stop = false;
	for (int j = pos; !stop && j < tokens.size(); ++j)
	{
		name = tables.get_name(tokens[j]);
		if (name == "var" || name == "const")
		{
			california.push_back((tokens[j]));
		}
		else
		{
			int r;
			do
			{
				r = road_map[dictionary[tables.get_name(texas.top())]][dictionary[name]];
				if (r == 1)
				{
					texas.push(tokens[j]);
				}
				else if (r == 2)
				{
					california.push_back(texas.top());
					texas.pop();
				}
				else if (r == 3)
				{
					texas.pop();
				}
				else if (r == 4)
				{
					texas.pop();
					stop = 1;
				}
			} while (r == 2);
		}
	}
	return california;
}

bool syntax_analyzer::post_procces(system_tables& tables, const vector<triad_index>& tokens)
{
	int pred_status = 0, status = 0; // 0 - passive, 1 - declare, 2 - expression
	for (int i = 0; i < tokens.size(); ++i)
	{
		if (tables.get_name(tokens[i]) == "int")
		{
			pred_status = 1;
			status = 1;
		}
		if (tables.get_name(tokens[i]) == "=")
		{
			tables.vars.change_type(2, tokens[i - 1]);
			status = 2;
		}
		if (tables.get_name(tokens[i]) == ";" || tables.get_name(tokens[i]) == "main")
		{
			status = 0;
			pred_status = 0;
		}
		if (tables.get_name(tokens[i]) == ",")
		{
			status = pred_status;
		}

		if (tables.get_name(tokens[i]) == "var")
		{
			if (tables.get_name(tokens[i - 1]) == ")")
			{
				status = 0;
				pred_status = 0;
			}
			int type = tables.vars.get_type(tokens[i]);

			if (status == 0)
			{
				if (type == 0)
				{
					cerr << "Variable " << tables.vars.get_name(tokens[i]) << " does not declare" << endl;
					return false;
				}
			}
			if (status == 1)
			{
				if (type != 0)
				{
					cerr << "Variable " << tables.vars.get_name(tokens[i]) << " was declaring before" << endl;
					return false;
				}
				else
				{
					tables.vars.change_type(1, tokens[i]);
				}
			}

			if (status == 2)
			{
				if (type == 0)
				{
					cerr << "Variable " << tables.vars.get_name(tokens[i]) << " does not declare" << endl;
					return false;
				}
				else if (type == 1)
				{
					cerr << "Variable " << tables.vars.get_name(tokens[i]) << " does not initilization" << endl;
					return false;
				}
			}
		}
	}
	return true;
}

vector<vector<triad_index>> syntax_analyzer::create_opz(system_tables& tables, vector<triad_index>& tokens)
{
	if (token_analyzer(tables, tokens) == -1)
	{
		return vector<vector<triad_index>>();
	}
	field_selection(tables, tokens);
	if (!post_procces(tables, tokens))
	{
		return vector<vector<triad_index>>();
	}

	vector <vector <triad_index>>  reverse_pols;
	stack <pair<int, vector <triad_index>>> st;
	stack <int> brackets;
	int acuum_for = 0;
	for (int i = 0; i < tokens.size(); ++i)
	{
		if (tables.get_name(tokens[i]) == "for")
		{
			i += 2;
			do
			{
				for (; tables.get_name(tokens[i]) != "=" && tables.get_name(tokens[i]) != ";"; ++i);
				if (tables.get_name(tokens[i]) == "=")
				{
					reverse_pols.push_back(get_opz(tables, tokens, i - 1));
					++i;
				}
			} while (tables.get_name(tokens[i]) != ";");
			++i;
			reverse_pols.push_back(get_opz(tables, tokens, i));
			for (; tables.get_name(tokens[i]) != ";"; ++i);
			++i;
			st.push({ reverse_pols.size() - 1,  get_opz(tables, tokens, i) });
			i += 2;
			do
			{
				for (; tables.get_name(tokens[i]) != "="
					&& tables.get_name(tokens[i]) != ";"
					&& tables.get_name(tokens[i]) != "{"
					&& tables.get_name(tokens[i]) != "for"; ++i);
				if (tables.get_name(tokens[i]) == "=")
				{
					reverse_pols.push_back(get_opz(tables, tokens, i - 1));
					++i;
				}
			} while (tables.get_name(tokens[i]) != ";"
				&& tables.get_name(tokens[i]) != "{"
				&& tables.get_name(tokens[i]) != "for");

			if (tables.get_name(tokens[i]) == "for")
			{
				++acuum_for;
				--i;
			}
			else if (tables.get_name(tokens[i]) == "{")
			{
				brackets.push(1); // возврат для цикла
			}
			else
			{
				++acuum_for;
				for (; acuum_for > 0; --acuum_for)
				{
					reverse_pols.push_back(st.top().second);
					reverse_pols[st.top().first].push_back({ -2, (int)reverse_pols.size(), 0 });
					reverse_pols[reverse_pols.size() - 1].push_back({ -3, st.top().first, 0 });
					st.pop();
				}
			}
		}
		else if (tables.get_name(tokens[i]) == "{")
		{
			brackets.push(0);
		}
		else if (tables.get_name(tokens[i]) == "}")
		{
			if (brackets.top())
			{
				++acuum_for;
				for (; acuum_for > 0; --acuum_for)
				{
					reverse_pols.push_back(st.top().second);
					reverse_pols[st.top().first].push_back({ -2, (int)reverse_pols.size(), 0 });
					reverse_pols[reverse_pols.size() - 1].push_back({ -3, st.top().first, 0 });
					st.pop();
				}
			}
			brackets.pop();
		}
		else if (tables.get_name(tokens[i]) == "=")
		{
			reverse_pols.push_back(get_opz(tables, tokens, i - 1));
		}
	}

	for (int i = 0; i < reverse_pols.size(); ++i)
	{
		for (int j = 0; j < reverse_pols[i].size(); ++j)
		{
			if (reverse_pols[i][j].cur_table == -2 || reverse_pols[i][j].cur_table == -3)
			{
				if (reverse_pols[i][j].index >= reverse_pols.size())
				{
					reverse_pols.push_back({ {-4, reverse_pols[i][j].index, 0} });
				}
				else
				{
					reverse_pols[reverse_pols[i][j].index].insert(reverse_pols[reverse_pols[i][j].index].begin(), { -4, reverse_pols[i][j].index, 0 });
				}
			}
		}
	}
	return reverse_pols;
}

void syntax_analyzer::print_reverse_pols(ofstream out, system_tables& tables, vector<vector<triad_index>> pols)
{
	for (int i = 0; i < pols.size(); ++i)
	{
		for (int j = 0; j < pols[i].size(); ++j)
		{
			stringstream name;
			if (pols[i][j].cur_table > 0)
			{
				name << tables.get_name(pols[i][j], 1);
			}
			else
			{
				if (pols[i][j].cur_table == -2)
				{
					name << "m" << pols[i][j].index << " USL";
				}
				else if (pols[i][j].cur_table == -3)
				{
					name << "m" << pols[i][j].index << " BP";
				}
				else if (pols[i][j].cur_table == -4)
				{
					name << "m" << pols[i][j].index << ":";
				}

			}
			out << name.str() << " ";
		}
		out << endl;
	}
}

