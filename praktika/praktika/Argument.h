#pragma once
#include <string>

class Argument {
public:
	Argument()
	{
		name = "";
		typeName = "";
		baseTypeName = "";
		kind = "";
		order = -1;
	}
	Argument(const Argument& arg)
	{
		name = arg.name;
		typeName = arg.typeName;
		baseTypeName = arg.baseTypeName;
		kind = arg.kind;
		order = arg.order;
	}

	Argument& operator = (const Argument& arg)
	{
		if (this != &arg)
		{
			name = arg.name;
			typeName = arg.typeName;
			baseTypeName = arg.baseTypeName;
			kind = arg.kind;
			order = arg.order;
		}
		return *this;
	}
	bool operator == (const Argument& arg)
	{
		if (this->baseTypeName != arg.baseTypeName)
			return false;
		if (this->kind != arg.kind)
			return false;
		if (this->name != arg.name)
			return false;
		if (this->order != arg.order)
			return false;
		if (this->typeName != arg.typeName)
			return false;
		return true;
	}
	bool operator != (const Argument& arg)
	{
		if (this->baseTypeName == arg.baseTypeName)
			return false;
		if (this->kind == arg.kind)
			return false;
		if (this->name == arg.name)
			return false;
		if (this->order == arg.order)
			return false;
		if (this->typeName == arg.typeName)
			return false;
		return true;
	}

	bool operator < (const Argument& arg) { return order < arg.order; }
	/*
	std::string GetName() const { return name; }
	std::string GetTypeName() const { return typeName; }
	std::string GetBaseTypeName() const { return baseTypeName; }
	std::string GetKind() const { return kind; }
	int GetOrder() const { return order; }

	void SetName(std::string Name) { name = Name; }
	void SetTypeName(std::string TypeName) { typeName = TypeName; }
	void SetBaseTypeName(std::string BaseTypeName) { baseTypeName = BaseTypeName; }
	void SetKind(std::string Kind) { kind = Kind; }
	void SetOrder(int Order) { order = Order; }*/

	std::string name;
	std::string typeName;
	std::string baseTypeName;
	std::string kind;
	int order;
};