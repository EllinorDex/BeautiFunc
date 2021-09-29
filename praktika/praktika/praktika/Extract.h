#pragma once
#include <string>
#include <list>

struct DefVal
{
	std::string name;
	std::string value;
};

struct Field
{
	std::string name;
	std::string baseTypeName;
	std::string typeName;
	std::string kind;
	int order;
};

struct Arg
{
	std::string name;
	std::string baseTypeName;
	std::string typeName;
	std::string kind;
	int order;

	std::list<DefVal> defVal;
	std::list<Field> Fields;
};


struct Result
{
	std::string fuctionName;
	std::string APICategory;
	std::string DLL;
	std::string returnType;
	std::list<Arg> arguments;
};

Result functionInformation(std::string functionName);

