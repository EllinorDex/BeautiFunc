#pragma once
#include <string>
#include <list>
#include <map>
#include "Argument.h"

struct pair
{
	std::string name;
	std::string value;
	bool operator == (const pair& p)
	{
		if (this->name != p.name)
			return false;
		if (this->value != p.value)
			return false;
		return true;
	}
};

class Type
{
public:
	std::string typeName;
	std::string baseTypeName;
	std::string kind;
	Type()
	{
		typeName = "";
		baseTypeName = "";
		kind = "";
	}
	Type(std::string tn, std::string btn, std::string k)
	{
		typeName = tn;
		baseTypeName = btn;
		kind = k;
		fields = std::list<Argument>();
		defval = std::list<pair>();
	}

	std::list<Argument> fields;
	std::list<pair> defval;
};

/*
class Set:public Type
{
public:
	Set() : Type()
	{
		fields = std::list<Argument>{};
	}
	Set(std::string tn, std::string btn, std::string k) : Type(tn, btn, k)
	{
		fields = std::list<Argument>();
	}

	std::list<Argument> fields;

};

class DefValue : public Type
{
public:
	std::string value;
	std::string name;
	DefValue():Type()
	{
		value = "";
		name = "";
	}
	DefValue(std::string tn, std::string btn, std::string k) : Type(tn, btn, k)
	{
		value = "";
		name = "";
	}
};
*/