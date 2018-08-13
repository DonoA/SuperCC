#pragma once
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <functional>
#include <iostream>

#include "statement.h"
#include "utils.h"

namespace skiff
{
	namespace environment
	{
		class scope;
		class skiff_value;
		class skiff_object;
		class skiff_function;
		class skiff_class;

		using skiff_func_sig = std::function<skiff_object(std::vector<skiff_object>,scope *)>;

		class skiff_object
		{
		public:
			skiff_object() : skiff_object(nullptr, nullptr) { }
			skiff_object(void * str, skiff_class * type);

			skiff_class * get_class();
			void set_class(skiff_class * clazz);
			std::string to_string();
			void * get_value();
			void set_value(void * v);
		private:
			skiff_class * type;
			void * value;
		};

		class skiff_function
		{
		public:
			struct function_parameter
			{
				skiff_class * typ;
				std::string name;
			};
			static function_parameter create_function_parameter(std::string name,
				skiff_class * typ);
			skiff_function() : skiff_function("", std::vector<function_parameter>(), nullptr, nullptr)
			{ };
			skiff_function(std::string name, std::vector<function_parameter> params,
				skiff_class * returns, scope * env);
			skiff_function(std::string name, scope * env, skiff_func_sig * builtin);
			skiff_object eval(skiff_object self);
			skiff_object eval(std::vector<skiff_object> params);
		private:
			scope * function_env;
			std::string name;
			std::vector<function_parameter> params;
			skiff_class * returns;
			skiff_func_sig * builtin;
		};

		class skiff_class
		{
		public:
			skiff_class() : skiff_class("") { }
			skiff_class(std::string name) : skiff_class(name, nullptr) { }
			skiff_class(std::string name, skiff_class * parent);
			scope * get_scope();
			std::string get_name();

			void add_operator(std::string key, skiff_function op);
			skiff_object invoke_operator(std::string op, std::vector<skiff_object> params);
			
			void add_constructor(skiff_function constructor_);
			skiff_object construct(std::vector<skiff_object> params);
		private:
			std::string name;
			skiff_class * parent;
			scope * class_env;
			skiff_function constructor;
			std::map<std::string, skiff_function> ops;
		};

		class scope
		{
		public:
			scope() : scope(nullptr) { };
			scope(scope * inherit);
			void define_variable(std::string name, skiff_object val);
			skiff_object get_variable(std::string name);
			void define_type(std::string name, skiff_class * cls);
			skiff_class * get_type(std::string name);
			void define_class_type(skiff_class * cls);
			skiff_class * get_class_type();
			void define_function(std::string name, skiff_function * func);
			skiff_function * get_function(std::string name);

			void print_debug();
			std::string get_debug_string();
		private:
			std::map<std::string, skiff_object> env;
			scope * inherit;
//			std::map<std::string, skiff_class> known_types;
//			std::map<std::string, skiff_function> known_functions;
		};
	}
}