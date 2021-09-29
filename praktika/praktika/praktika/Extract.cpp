#include "Extract.h"
#include "SQLFunc.h"

Result functionInformation(std::string functionName)
{
    Result result;
    Function func = extract_from_db(functionName);

    result.APICategory = func.Get_API_Categorie();
    result.DLL = func.Get_DLL();
    result.returnType = func.Get_Return_Type();
    result.fuctionName = functionName;


    std::list<Argument> arguments = func.Get_Arguments();
    for (auto i = arguments.begin(); i != arguments.end(); ++i)
    {
        Arg arg;
        arg.baseTypeName = i->GetBaseTypeName();
        arg.kind = i->GetKind();
        arg.name = i->GetName();
        arg.order = i->GetOrder();
        arg.typeName = i->GetTypeName();

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
                Field w;
                w.baseTypeName = j->GetBaseTypeName();
                w.kind = j->GetKind();
                w.name = j->GetName();
                w.order = j->GetOrder();
                w.typeName = j->GetTypeName();
                arg.Fields.push_back(w);
            }
        }
        result.arguments.push_back(arg);
    }
    return result;
}