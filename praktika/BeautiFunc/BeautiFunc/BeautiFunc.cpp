#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include "Extract.h"
#include <stdio.h>


#define MAX_TRIAL_RUNS 5

const TCHAR szCounterFileName[] = L"counter.dat";


VOID PrinterFuncInfo(std::string funcName, std::string* args, DWORD argc) {
    Result* q = functionInformation(funcName);
    const char* str = "there!";
    printf("%-20s %s\n","Function name:", funcName.c_str());
    printf("%-20s %s\n", "DLL name:", q->DLL.c_str());
    printf("%-20s %s\n", "API Category:", q->APICategory.c_str());
    printf("%-20s %s\n\n", "Return type:", q->returnType.c_str());
    printf("Arguments:\n");

    for (auto arg = q->arguments.begin(); arg != q->arguments.end(); ++arg) {
        printf("%10d.\n", arg->order + 1);
        printf("%12s%-15s%s\n", " ","Name:", arg->name.c_str());
        printf("%12s%-15s%s\n", " ", "Type:", arg->typeName.c_str());
        printf("%12s%-15s%s\n", " ", "Raw value:", args[arg->order].c_str());
        printf("%12s%-15s%s\n", " ", "Kind:", arg->kind.c_str());
        std::string pointer = "->";
        std::string alias = "~~";
        std::string type_chain = arg->typeName;
        if (arg->kind == "Alias") {
            type_chain += alias;
            type_chain += arg->baseTypeName;
        }
        if (arg->kind == "Pointer") {
            type_chain += pointer;
            type_chain += arg->baseTypeName;
        }
        auto baseArg = arg->baseType;
        while (baseArg) {

            //printf("%s", baseArg->typeName.c_str());
            if (baseArg->kind == "Alias") {
                type_chain += alias;
                type_chain += baseArg->baseTypeName;
            }
            if (baseArg->kind == "Pointer") {
                type_chain += pointer;
                type_chain += baseArg->baseTypeName;
            }
            baseArg = baseArg->baseType;
        }
        printf("%12s%-15s%s\n", " ", "Type Chain:", type_chain.c_str());
        printf("%12s%-15s%s\n\n", " ", "Beauti value:", args[arg->order].c_str());
    }
}


int main() {
    DWORD dwCounter, dwTemp;
    ///*HANDLE hFile = CreateFile(szCounterFileName, GENERIC_WRITE, 0, NULL,
    //    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);*/
    HANDLE hFile = CreateFile(szCounterFileName, GENERIC_READ, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) {
        return 1;
    }

    ReadFile(hFile, &dwCounter, sizeof(dwCounter), &dwTemp, NULL);

    std::string funcName = "ReadFile";
    char str[16 + 1];
    std::string args[5]{};

    sprintf(str, "%p", hFile);
    args[0] = str;
    sprintf(str, "%p", &dwCounter);
    args[1] = str;
    args[2] = std::to_string(sizeof(dwCounter));
    sprintf(str, "%p", &dwTemp);
    args[3] = str;
    args[4] = "NULL";
    PrinterFuncInfo(funcName, args, 5);
    ExitProcess(0);
}
