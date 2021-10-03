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
	std::string typeName;
	std::string baseTypeName;
	std::string kind;
	std::list<Argument> fields;
	std::list<pair> defval;
public:
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

	void SetTypeName(std::string tn) { typeName = tn; };
	void SetBaseTypeName(std::string btn) { baseTypeName = btn; }
	void SetKind(std::string k) { kind = k; }
	void SetFields(std::list<Argument> arg) { fields = arg; }
	void SetDefVal(std::list<pair> dv) { defval = dv; }

	std::string GetTypeName() const { return typeName; }
	std::string GetBaseTypeName() const { return baseTypeName; }
	std::string GetKind() const { return kind; }
	std::list<Argument> GetFields() const { return fields; }
	std::list<pair> GetDefVal() const { return defval; }

	void PushDefVal(pair p) { defval.push_back(p); }
	void PushField(Argument arg) { fields.push_back(arg); }
	void SortFields() { fields.sort();}
};
