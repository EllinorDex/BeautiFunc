#include <iostream>
#include "Extract.h"

int main(int argc, char* argv[]) {
    // primer
    Result* q = functionInformation("ReadFile");
    std::cout << q->APICategory << std::endl;
    std::cout << q->DLL << std::endl;
    std::cout << q->returnType << std::endl;
    std::cout << "\n///\n" << std::endl;
    for (auto i = q->arguments.begin(); q->arguments.end() != i; ++i)
    {
        std::cout << i->baseTypeName << std::endl;
        std::cout << i->baseType->kind << std::endl;

        if (i->baseType->kind == "Struct")
        {
            for (auto j = i->baseType->Fields.begin(); i->baseType->Fields.end() != j; ++j)
            {
                std::cout << "\n---------\n" << std::endl;
                std::cout << j->name << std::endl;
                std::cout << j->kind << std::endl;
                std::cout << j->typeName << std::endl;
                std::cout << j->order << std::endl;

                std::cout << "\n---------\n" << std::endl;
            }
        }
        std::cout << i->name << std::endl;
        std::cout << i->kind << std::endl;
        std::cout << i->typeName << std::endl;
        std::cout << i->order << std::endl;
        std::cout << "\n______\n" << std::endl;
    }
    std::cout << q->arguments.begin()->name << std::endl;
    std::cout << q->arguments.begin()->baseTypeName << std::endl;
    delete q;
    //
    return 0;
}
