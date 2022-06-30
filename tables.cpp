#include "tables.h"


bool constant_table::add_element(string obj)
{
	if (contained_in_table(obj) < 0)
	{
		table.push_back(obj);
		sort(table.begin(), table.end());
		return true;
	}
	return false;
}

int constant_table::contained_in_table(string obj)
{
	for (int i = 0; i < table.size(); ++i)
	{
		if (table[i] == obj)
			return i;
	}
	return -1;
}

constant_table::constant_table()
{}

string constant_table::get_name(triad_index el)
{
	return table[el.index];
}

unsigned int variable_table::calc_hash(string obj)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	for (unsigned int i = 0; i < obj.length(); i++)
	{
		hash = hash * a + obj[i];
		a = a * b;
	}
	return (hash & 0x7FFFFFFF);
}

pair <int, int> variable_table::contained_in_table(variable obj, int hash)
{
	if (objects[hash].size() == 0)
	{
		return { -1, 0 };
	}
	for (int i = 0; i < objects[hash].size(); ++i)
	{
		if (equal_object(objects[hash][i], obj))
		{
			return { hash, i };
		}
	}
	return { -1, 0 };
}

bool variable_table::equal_object(variable obj_1, variable obj_2)
{
	if (obj_1.name != obj_2.name || obj_1.type != obj_2.type || obj_1.value != obj_2.value)
	{
		return false;
	}
	return true;
}

pair <int, int> variable_table::contained_in_table(string obj_name)
{
	int hash = calc_hash(obj_name) % objects.size();
	if (objects[hash].size() == 0)
	{
		return { -1, 0 };
	}
	for (int i = 0; i < objects[hash].size(); ++i)
	{
		if (objects[hash][i].name == obj_name)
		{
			return { hash, i };
		}
	}
	return { -1, 0 };
}

bool variable_table::change_value(string value, int hash, int p)
{
	if (hash >= 0 && hash < objects.size() && p >= 0 && p < objects[hash].size())
	{
		objects[hash][p].value = value;
		return true;
	}
	return false;
}

bool variable_table::change_type(int type, int hash, int p)
{
	if (hash >= 0 && hash < objects.size() && p >= 0 && p < objects[hash].size())
	{
		objects[hash][p].type = type;
		return true;
	}
	return false;
}

bool variable_table::change_value(string value, pair<int, int> ind)
{
	return change_value(value, ind.first, ind.second);
}

bool variable_table::change_type(int type, triad_index tr)
{
	return change_type(type, tr.index, tr.sub_index);
}

bool variable_table::set_local_index(triad_index el, pair<int, int> value)
{
	if (el.index >= 0 && el.index < objects.size() && el.sub_index >= 0 && el.sub_index < objects[el.index].size())
	{
		objects[el.index][el.sub_index].local_adress = { value };
		return true;
	}
	return false;
}

int variable_table::get_type(triad_index tr)
{
	if (tr.index < objects.size() && tr.sub_index < objects[tr.index].size())
	{
		return objects[tr.index][tr.sub_index].type; // ind.first - адрес в хеш-таблице, адрес в колизии 
	}
	return -1;
}

bool variable_table::get_value(string& value, int hash, int p)
{
	if (hash >= 0 && hash < objects.size() && p >= 0 && p < objects[hash].size())
	{
		value = objects[hash][p].value;
		return true;
	}
	return false;
}

bool variable_table::get_value(string& value, pair<int, int> ind)
{
	return get_value(value, ind.first, ind.second);
}

pair<int, int> variable_table::get_local_index(triad_index el)
{
	if (el.index >= 0 && el.index < objects.size() && el.sub_index >= 0 && el.sub_index < objects[el.index].size())
	{

		return objects[el.index][el.sub_index].local_adress;
	}
	return { -1, 0 };
}

pair<int, int> variable_table::contained_in_table(variable obj)
{
	return contained_in_table(obj, calc_hash(obj.name));
}

pair <int, int> variable_table::add_element(variable obj)
{
	unsigned int hash = calc_hash(obj.name) % objects.size();
	pair <int, int> index = contained_in_table(obj, hash);
	if (index.first == -1)
	{
		objects[hash].push_back(obj);
		return { hash, objects[hash].size() - 1 };
	}
	return { index.first, index.second };
}

pair<int, int> variable_table::add_element(string obj)
{
	return add_element(variable(obj, 0, ""));
}

int variable_table::print_table(ofstream f)
{
	f << objects.size() << endl;
	for (int i = 0; i < objects.size(); ++i)
	{
		for (int j = 0; j < objects[i].size(); ++j)
		{
			f << i << " " << j << " " << objects[i][j].name << " " << objects[i][j].type << " " << objects[i][j].value << " " << objects[i][j].local_adress.first << " " << objects[i][j].local_adress.second << "\n";
		}
	}
	return 0;
}

vector<variable> variable_table::get_all_elems()
{
	vector<variable> result;
	for (int i = 0; i < objects.size(); ++i)
	{
		for (int j = 0; j < objects[i].size(); ++j)
		{
			result.push_back(objects[i][j]);
		}
	}
	return result;
}


variable_table::variable_table(int size_table) : objects(size_table, vector <variable>(0))
{}

variable_table::variable_table(string file_name)
{
	ifstream file(file_name);
	int size;
	file >> size;
	objects.resize(size);
	int i, j;
	variable var;
	while (file >> i >> j)
	{
		if (j >= objects[i].size())
		{
			objects[i].resize(j + 1);
		}
		file >> objects[i][j].name >> objects[i][j].type >> objects[i][j].value;
	}
}

string variable_table::get_name(triad_index el)
{
	return objects[el.index][el.sub_index].name;
}

variable::variable(string n, int t, string v)
{
	name = n;
	type = t;
	value = v;
	local_adress = { -1, 0 };
}

variable::variable()
{
	name = "";
	type = 0;
	local_adress = { -1, 0 };
}

void system_tables::init()
{
	key_words.add_element("int");
	key_words.add_element("for");

	operators.add_element("+");
	operators.add_element("-");
	operators.add_element("*");
	operators.add_element("&&");
	operators.add_element("||");
	operators.add_element(">=");
	operators.add_element("<=");
	operators.add_element(">");
	operators.add_element("<");
	operators.add_element("==");
	operators.add_element("=");

	separators.add_element(",");
	separators.add_element(";");
	separators.add_element("{");
	separators.add_element("}");
	separators.add_element("(");
	separators.add_element(")");
}

system_tables::system_tables(int size_tables) : vars(size_tables), constants(size_tables)
{
	init();
}

system_tables::system_tables(string const_table, string var_table) : vars(var_table), constants(const_table)
{
	init();
}

string system_tables::get_name(triad_index el, int mode)
{
	string spike;
	switch (el.cur_table)
	{
	case KEY_WORD:
		return key_words.get_name(el);
	case OPERATOR:
		return operators.get_name(el);
	case SEPARATOR:
		return separators.get_name(el);
	case VARIABLE:
		spike = vars.get_name(el);
		if (!mode)
		{
			return spike == "main" ? "main" : "var";
		}
		return spike;
	case CONSTANT:
		if (!mode)
		{
			return "const";
		}
		return constants.get_name(el);
	}
	return "Not found!";
}

triad_index::triad_index(int c_t, int i, int s_i)
{
	cur_table = c_t;
	index = i;
	sub_index = s_i;
}

triad_index::triad_index(int c_t, pair<int, int> par_index) :triad_index(c_t, par_index.first, par_index.second)
{}

vector<triad_index> triad_index::read_tokens(ifstream file)
{
	triad_index el(0, 0);
	vector <triad_index> result;
	while (file >> el.cur_table >> el.index >> el.sub_index)
	{
		result.push_back(el);
	}
	return result;
}

bool triad_index::operator==(const triad_index right)
{
	return cur_table == right.cur_table && index == right.index && sub_index == right.sub_index;
}
