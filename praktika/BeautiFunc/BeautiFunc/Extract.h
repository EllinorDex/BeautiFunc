#pragma once
#include <string>
#include <list>

struct DefVal
{
	std::string name;
	std::string value;
};

struct Arg
{

	std::string name;
	std::string baseTypeName;
	Arg* baseType;
	std::string typeName;
	std::string kind;
	int order;

	std::list<DefVal> defVal;
	std::list<Arg> Fields;
	
	void distr()
	{
		if (baseType != NULL)
		{
			baseType->distr();
			delete baseType;
		}
		for (auto i = Fields.begin(); i != Fields.end(); ++i)
		{
			i->distr();
		}
	}
	
};


struct Result
{
	std::string fuctionName;
	std::string APICategory;
	std::string DLL;
	std::string returnType;
	std::list<Arg> arguments;
	~Result()
	{
		for (auto i = arguments.begin(); i != arguments.end(); i++)
		{
			i->distr();
		}
	}
};

Result* functionInformation(std::string functionName);