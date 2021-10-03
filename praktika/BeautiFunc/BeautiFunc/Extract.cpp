#include "Extract.h"
#include "SQLFunc.h"

Result* functionInformation(std::string functionName)
{
    Result* result = new Result;
    Function func = extract_from_db(functionName);

    result->APICategory = func.Get_API_Categorie();
    result->DLL = func.Get_DLL();
    result->returnType = func.Get_Return_Type();
    result->fuctionName = functionName;


    std::list<Argument> arguments = func.Get_Arguments();
    for (auto i = arguments.begin(); i != arguments.end(); ++i)
    {
        Arg arg;
        arg.baseTypeName = i->GetBaseTypeName();
        arg.kind = i->GetKind();
        arg.typeName = i->GetTypeName();
        arg.name = i->GetName();
        arg.order = i->GetOrder();
        arg.baseType = new Arg(typeInformation(arg.baseTypeName));

        Type temp = typeDisclosure(*i);

        if (arg.kind == "Alias")
        {
            std::list<pair> q = temp.GetDefVal();
            for (auto j = q.begin(); j != q.end(); ++j)
            {
                DefVal w;
                w.name = j->name;
                w.value = j->value;
                arg.defVal.push_back(w);
            }
        }
        if (arg.kind == "Struct" || arg.kind == "Union")
        {
            std::list<Argument> e = temp.GetFields();
            for (auto j = e.begin(); j != e.end(); ++j)
            {
                Arg w = typeInformation(j->GetTypeName());
                w.order = j->GetOrder();
                w.name = j->GetName();
                arg.Fields.push_back(w);
            }
        }
        result->arguments.push_back(arg);
    }
    return result;
}
