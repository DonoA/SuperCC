#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <fstream>
#include "../Core/types.h"
#include "../Core/statement.h"
#include "../Core/parsers.h"
#include "../Core/utils.h"
#include "../Core/builtin.h"

using std::string;
using std::map;
using std::stack;
using std::vector;

template<class T>
type_class define_native_builtins(scope * env, builtin::type nt)
{
	type_class t = type_class(builtin::get_name_for(nt), builtin::get_id_for(nt));
	(*t.get_operators())[string(1, '+')] = function("add", env, navite_builtin::create_add<T>());
	(*t.get_operators())[string(1, '-')] = function("sub", env, navite_builtin::create_sub<T>());
	(*t.get_operators())[string(1, '*')] = function("mul", env, navite_builtin::create_mul<T>());
	(*t.get_operators())[string(1, '/')] = function("div", env, navite_builtin::create_div<T>());
	t.get_scope()->define_function("to_string", function("to_string", env, navite_builtin::create_to_string<T>()));
	t.get_scope()->define_function("clone", function("clone", env, navite_builtin::create_clone<T>()));
	return t;
}

template<class T>
type_class define_native_fixpoint_builtins(scope * env, builtin::type nt)
{
	type_class t = define_native_builtins<T>(env, nt);
	(*t.get_operators())[string(1, '%')] = function("mod", vector<function::function_parameter>(), type_class(), env, navite_builtin::create_mod<T>());
	return t;
}

type_class define_string_builtins(scope * env)
{
	type_class t = type_class("String", builtin::type::String);
	(*t.get_operators())[string(1, '+')] = function("add", env, navite_builtin::create_add<string>());
	t.get_scope()->define_function("to_string", function("to_string", env, new std::function<object*(object*, vector<object*>, scope*)>(&string_builtin::to_string)));
	t.get_scope()->define_function("clone", function("clone", env, new std::function<object*(object*, vector<object*>, scope*)>(&string_builtin::clone)));
	return t;
}

void load_standards(scope * env)
{
	env->define_type(builtin::get_name_for(builtin::type::Char), define_native_fixpoint_builtins<char>(env, builtin::type::Char));
	env->define_type(builtin::get_name_for(builtin::type::Int), define_native_fixpoint_builtins<int>(env, builtin::type::Int));
	env->define_type(builtin::get_name_for(builtin::type::Long), define_native_fixpoint_builtins<long>(env, builtin::type::Long));
	env->define_type(builtin::get_name_for(builtin::type::Float), define_native_builtins<float>(env, builtin::type::Float));
	env->define_type(builtin::get_name_for(builtin::type::Double), define_native_builtins<double>(env, builtin::type::Double));
	env->define_type(builtin::get_name_for(builtin::type::String), define_string_builtins(env));
}

bool handle_line(string input, char c, queue<statement *> * stmts)
{
	input = remove_pad(input);
	if (input != "")
	{
		statement * stmt = parse_statement(input);
		stmts->push(stmt);
	}
	if (c == '}')
	{
		stmts->push(new end_block_statement());
	}
	//if (c == '}')
	//{
	//	stmts->push(new end_block_statement());
	//}
	if (input == "exit()")
	{
		return false;
	}
	return true;
}

void interactive_mode()
{
	queue<statement *> statements;

	bool running = true;
	while (running)
	{
		string input = string();
		std::cout << ">> ";
		bool parsing_string = false;
		char c;
		do {
			c = getchar();
			if ((c == '"' || c == '\'') && (input.length() == 0 || input[input.length() - 1] != '\\'))
			{
				parsing_string = !parsing_string;
			}
			if (parsing_string || (c != ';' && c != '{' && c != '}'))
			{
				input += c;
			}
		} while ((c != ';' && c != '{' && c != '}') || parsing_string);
		running = handle_line(input, c, &statements);
	}

	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << generate_indent(indent) << s->parse_string() << std::endl;
		indent += s->indent_mod();
	}
}

queue<statement *> parse_file(string infile)
{
	queue<statement *> statements;

	std::fstream fin(infile, std::fstream::in);
	bool running = true;
	string input = string();
	stack<char> braces;
	//bool parsing_string = false;
	char c = '\0';
	while (fin >> std::noskipws >> c && running) {
		if (!braces.empty() || (c != ';' && c != '{' && c != '}'))
		{
			input += c;
		}
		if ((c == ';' || c == '{' || c == '}') && braces.empty())
		{
			running = handle_line(input, c, &statements);
			input = string();
			c = '\0';
		}
		track_braces(input.length() == 0 ? '\0' : input[input.length() - 1], c, &braces);
	}
	return statements;
}

void print_parse(queue<statement *> statements)
{
	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << generate_indent(indent) << s->parse_string() << std::endl;
		indent += s->indent_mod();
	}
}

void translate_c(queue<statement *> statements)
{
	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << generate_indent(indent) << s->eval_c();
		indent += s->indent_mod();
		if (s->indent_mod() == 0)
		{
			std::cout << ";" << std::endl;
		}
		else if (s->indent_mod() == 1)
		{
			std::cout << std::endl << "{" << std::endl;
		}
		else
		{
			std::cout << "}" << std::endl;
		}
	}
}

void evaluate(scope * env, queue<statement *> statements)
{

	size_t indent = 0;
	while (!statements.empty())
	{
		statement * s = statements.front();
		statements.pop();
		std::cout << s->eval(env) << std::endl;
		indent += s->indent_mod();
	}
}


scope env;

int main()
{
	load_standards(&env);
	//interactive_mode();
	queue<statement *> test_statements = parse_file("test.su");
	print_parse(test_statements);
	//evaluate(test_statements);
	//system("pause");
	//queue<statement *> trans_statements = parse_file("pos_c.su");
	//print_parse(trans_statements);
	//translate_c(trans_statements);
	system("pause");
}

