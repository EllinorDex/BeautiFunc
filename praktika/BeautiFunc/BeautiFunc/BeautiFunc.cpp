#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include "Extract.h"
#include <stdio.h>
#include <algorithm>


const TCHAR szCounterFileName[] = L"counter.dat";


VOID PrinterFuncInfo(std::string funcName, std::string* args, DWORD argc)
{
    std::string pointer    = "->";
    std::string alias      = "~~";
    std::string type_chain = "";
    std::string beautiVal  = "";
    std::string beautiValPrev = "";
    void* pointVal         = 0;
    Arg* baseArg           = 0;
    int countPars          = 0;
    int rawVal             = 0;
    char* str;
    std::string buf;

    auto q = functionInformation(funcName);

    printf("%-20s %s\n","Function name:", funcName.c_str());
    printf("%-20s %s\n", "DLL name:", q->DLL.c_str());
    printf("%-20s %s\n", "API Category:", q->APICategory.c_str());
    printf("%-20s %s\n\n", "Return type:", q->returnType.c_str());
    printf("Arguments:\n");
    
    for (auto arg = q->arguments.begin(); arg != q->arguments.end(); ++arg)
    {
        printf("%10d.\n", arg->order + 1);
        printf("%12s%-15s%s\n", " ","Name:", arg->name.c_str());
        printf("%12s%-15s%s\n", " ", "Type:", arg->typeName.c_str());
        /*sscanf(args[arg->order].c_str(), "%", &rawVal);*/
        printf("%12s%-15s%s\n", " ", "Raw value:", args[arg->order].c_str());
        printf("%12s%-15s%s\n", " ", "Kind:", arg->kind.c_str());

        type_chain = arg->typeName;

        baseArg = &*arg;
        beautiVal = args[arg->order];
        beautiValPrev = beautiVal;
        countPars = 0;
        str = (char*)args[arg->order].c_str();

        while (baseArg->baseType)
        {
            if (baseArg->kind == "Alias")
            {
                type_chain += alias;
                type_chain += baseArg->baseTypeName;
            }

            if (baseArg->kind == "Pointer")
            {
                sscanf(str, "0x%p", &pointVal);
                if (pointVal == NULL)
                {
                    beautiValPrev = "NULL";
                    beautiVal += " (NULL)";
                    break;
                }
                else if (baseArg->baseTypeName == "VOID" || baseArg->baseTypeName == "void")
                {
                    beautiValPrev = "VOID";
                    --countPars;
                }
                else
                {
                    sprintf(str, "0x%016X", *(int*)pointVal);
                    beautiVal += " (" + std::string(str);
                    beautiValPrev = str;
                }

                type_chain += pointer;
                type_chain += baseArg->baseTypeName;
                ++countPars;

            }

            baseArg = baseArg->baseType;
        }

        if (baseArg->kind == "Integer")
        {
            int buf1;
            sscanf(beautiValPrev.c_str(), "0x%x", &buf1);
            beautiVal += " (" + std::to_string(buf1) + ")";
            printf("%12s%-15s%s\n", " ", "Type Chain:", type_chain.c_str());
        }
        else if (baseArg->kind == "")
        {

        }
        else if (baseArg->kind == "")
        {

        }
        else if (baseArg->kind == "")
        {

        }
        else if (baseArg->kind == "")
        {

        }
        else if (baseArg->kind == "")
        {

        }
        else if (baseArg->kind == "")
        {

        }

        beautiVal += std::string(countPars,')');
        printf("%12s%-15s%s\n", " ", "Type Chain:", type_chain.c_str());
        printf("%12s%-15s%s\n\n", " ", "Beauti value:", beautiVal.c_str());
    }
}


int main()
{
    DWORD dwCounter, dwTemp;
    ///*HANDLE hFile = CreateFile(szCounterFileName, GENERIC_WRITE, 0, NULL,
    //    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);*/
    HANDLE hFile = CreateFile(szCounterFileName, GENERIC_READ, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        return 1;
    }

    auto result = ReadFile(hFile, &dwCounter, sizeof(dwCounter), &dwTemp, NULL);

    std::string funcName = "ReadFile";
    char str[16 + 1];
    std::string args[5]{};

    sprintf(str, "0x%016X", hFile);
    args[0] = str;
    sprintf(str, "0x%p", &dwCounter);
    args[1] = str;
    sprintf(str, "0x%016X", sizeof(dwCounter));
    args[2] = str;
    std::cout << dwTemp << std::endl;
    sprintf(str, "0x%p", &dwTemp);
    args[3] = str;
    sprintf(str, "0x%016X", 0);
    args[4] = str;
    PrinterFuncInfo(funcName, args, 5);
    ExitProcess(0);
}
