#pragma once
#include "Type.h"
#include "Function.h"
#include "Argument.h"
#include <string>
Type typeDisclosure(Argument arg);
Function extract_from_db(std::string name);
