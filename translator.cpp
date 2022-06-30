#include "translator.h"

int translator::oper_ptr = 0;

string translator::get_assembler_var(system_tables& tables, triad_index el, string reg, int& op_ptr)
{
	stringstream result;
	if (el.cur_table == CONSTANT)
	{
		result << "MOV " << reg << ", " << tables.constants.get_name(el) << endl;
	}
	else if (el.cur_table == VARIABLE)
	{
		pair <int, int> loc = tables.vars.get_local_index(el);
		if (loc.first == 0 && loc.second == -1)
		{
			result << "MOV " << reg << ", " << tables.vars.get_name(el) << endl;
		}
		else
		{
			int index = loc.first + loc.second;
			result << "MOV " << reg << ", DWORD PTR[_STACK + " << index * 4 << "]" << endl;
		}
	}
	else if (el.cur_table == -6) //берем результат из стека в ассемблере
	{
		--op_ptr;
		result << "MOV " << reg << ", DWORD PTR[_STACK + " << op_ptr * 4 << "]" << endl;

	}
	return result.str();
}

string translator::init_data_memory(system_tables& tables, const vector<vector<triad_index>> opz)
{
	int maxlocal = 0;
	int maxoper = 0;

	int size_ = opz.size();
	for (int i = 0; i < size_; ++i)
	{
		int var_count = 0;
		int size = opz[i].size();
		for (int j = 0; j < size; ++j)
		{
			string token_name = tables.get_name(opz[i][j]);
			if (token_name == "var")
			{
				pair <int, int> p = tables.vars.get_local_index(opz[i][j]);
				if (maxlocal < p.first + p.second + 1)
				{
					maxlocal = p.first + p.second + 1;
				}
			}
			if (token_name == "var" || token_name == "const")
			{
				++var_count;
			}
		}
		if (maxoper < var_count)
		{
			maxoper = var_count;
		}
	}

	oper_ptr = maxlocal;
	stringstream result;
	result << ".DATA" << endl;
	result << ";block vars declare" << endl;
	vector <variable> vars = tables.vars.get_all_elems();

	int size = vars.size();
	for (int i = 0; i < size; ++i)
	{
		if (vars[i].local_adress.first == 0 && vars[i].local_adress.second == -1) // first - display , second - shift
		{
			result << vars[i].name << " DD ?" << endl;
		}
	}

	result << ";stack declare" << endl;
	result << "_STACK DD " << maxlocal + maxoper << " dup (0)" << endl;

	result << ";Pointer at the beginning of the operator stack" << endl;
	result << "_OPERPTR" << " DD " << oper_ptr << endl;

	return result.str();
}

string translator::init_main_process(system_tables& tables, const vector<vector<triad_index>> opz)
{
	stringstream result;
	stack <triad_index> temp;
	int jump_count = 0;
	result << ".CODE" << endl;
	result << "MAIN PROC" << endl;
	int size_ = opz.size();
	for (int i = 0; i < size_; ++i)
	{
		int stack_now = oper_ptr;
		int size = opz[i].size();
		for (int j = 0; j < size; ++j)
		{
			string token_name = tables.get_name(opz[i][j]);
			if (token_name == "var" || token_name == "const")
			{
				temp.push(opz[i][j]);
			}
			else if (opz[i][j].cur_table == OPERATOR)
			{
				triad_index right = temp.top();
				temp.pop();

				if (token_name != "=")
				{
					triad_index left = temp.top();
					temp.pop();
					if (left == right)
					{
						result << get_assembler_var(tables, right, "EBX", stack_now);
						result << get_assembler_var(tables, left, "EAX", stack_now);
					}
					else
					{
						result << get_assembler_var(tables, left, "EAX", stack_now);
						result << get_assembler_var(tables, right, "EBX", stack_now);
					}
				}
				else
				{
					result << get_assembler_var(tables, right, "EBX", stack_now);
				}
				if (token_name == "+")
				{
					result << "ADD EAX, EBX" << endl;
				}
				else if (token_name == "-")
				{
					result << "SUB EAX, EBX" << endl;
				}
				else if (token_name == "*")
				{
					result << "IMUL EBX" << endl;
				}
				else if (token_name == "/")
				{
					result << "MOV EDX, 0" << endl;
					result << "IDIV EBX" << endl;
				}
				else if (token_name == "&&")
				{
					result << "AND EAX, EBX" << endl;
				}
				else if (token_name == "||")
				{
					result << "OR EAX, EBX" << endl;
				}
				else if (token_name == "=")
				{
					triad_index left = temp.top();
					temp.pop();
					pair <int, int> loc = tables.vars.get_local_index(left);
					if (loc.first == 0 && loc.second == -1)// на нулевом уровне - глобальные переменные
					{
						result << "MOV " << tables.vars.get_name(left) << ", EBX" << endl;
					}
					else //на остальных уровнях - локальные переменные
					{
						int index = loc.first + loc.second;
						result << "MOV " << "DWORD PTR[_STACK + " << index * 4 << "], EBX" << endl;
					}
				}
				else
				{
					result << "CMP EAX, EBX" << endl;
					if (token_name == ">")
					{
						result << "JG L" << jump_count << endl;
					}
					else if (token_name == "<")
					{
						result << "JL L" << jump_count << endl;
					}
					else if (token_name == ">=")
					{
						result << "JGE L" << jump_count << endl;
					}
					else if (token_name == "<=")
					{
						result << "JLE L" << jump_count << endl;
					}
					else if (token_name == "==")
					{
						result << "JE L" << jump_count << endl;
					}
					result << "MOV EAX, 0" << endl;
					result << "JMP L" << jump_count + 1 << endl;
					result << "L" << jump_count << ":" << endl;
					result << "MOV EAX, 1" << endl;
					result << "L" << jump_count + 1 << ":" << endl;
					jump_count += 2;
				}

				if (token_name != "=")
				{
					temp.push({ -6, 0, 0 });
					result << "MOV DWORD PTR[_STACK + " << stack_now * 4 << "], " << "EAX" << endl;
					++stack_now;
				}
			}
			else if (opz[i][j].cur_table == -4)
			{
				result << "M" << opz[i][j].index << ":" << endl;
			}
			else if (opz[i][j].cur_table == -3)
			{
				result << "JMP M" << opz[i][j].index << endl;
			}
			else if (opz[i][j].cur_table == -2)
			{
				if (stack_now > oper_ptr) // вычёркиваем последнюю запись из стека ассемблера и используем её для выхода из цикла
				{
					--stack_now;
					result << "MOV " << "EAX" << ", DWORD PTR[_STACK + " << stack_now * 4 << "]" << endl;
				}
				else
				{
					result << get_assembler_var(tables, temp.top(), "EAX", stack_now);
					temp.pop();
				}
				result << "CMP EAX, 0" << endl;
				result << "JE M" << opz[i][j].index << endl;
			}
		}
	}
	result << "PUSH 0" << endl;
	result << "CALL ExitProcess@4" << endl;
	result << "MAIN ENDP" << endl;
	result << "END MAIN" << endl;
	return result.str();
}

string translator::get_asm_code(system_tables& tables, const vector<vector<triad_index>> opz)
{
	stringstream result;
	result << ".386" << endl;
	result << ".MODEL FLAT, STDCALL" << endl;
	result << "EXTRN ExitProcess@4:PROC" << endl;
	result << init_data_memory(tables, opz);
	result << init_main_process(tables, opz);
	return result.str();
}
