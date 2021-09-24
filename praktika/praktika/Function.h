#pragma once
#include "Argument.h"
#include <list>
class Function {
public:
	Function()
	{
		API_Categorie  = "";
		DLL = "";
		Name = "";
		Return_Type = "";
		Arguments = std::list<Argument>();
	}

	Function(const Function& func)
	{
		API_Categorie = func.API_Categorie;
		DLL = func.DLL;
		Name = func.Name;
		Return_Type = func.Return_Type;
		Arguments = func.Arguments;
	}
	Function& operator = (const Function& func)
	{
		if (this != &func)
		{
			API_Categorie = func.API_Categorie;
			DLL = func.DLL;
			Name = func.Name;
			Return_Type = func.Return_Type;
			Arguments = func.Arguments;
		}
		return *this;
	}
/*
	void SortArg() { Arguments.sort(); }

	std::string Get_API_Categorie() const { return API_Categorie; }
	std::string Get_DLL() const { return DLL; }
	std::string Get_Name() const { return Name; }
	std::string Get_Return_Type() const { return Return_Type; }
	std::list<Argument> Get_Arguments() const { return Arguments; }

	void Set_API_Categorie(std::string temp) { API_Categorie = temp; }
	void Set_DLL(std::string temp) { DLL = temp; }
	void Set_Name(std::string temp) { Name = temp; }
	void Set_Return_Value(std::string temp) { Return_Type = temp; }
	void Set_Arguments(std::list<Argument> temp) { Arguments = temp; }*/

	std::string API_Categorie;
	std::string DLL;
	std::string Name;
	std::string Return_Type;
	std::list<Argument> Arguments;
};