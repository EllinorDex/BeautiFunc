#include <iostream>
#include "sqlite/sqlite3.h"
#include <string>
#include "Function.h"
#include <stdlib.h>
#include "Type.h"
#include <algorithm>

// функция для sql запроса поиска заданных значений
static int DefValueCallback(void* result, int argc, char** argv, char** azColName)
{
    pair p;
    p.name = argv[3] ? argv[3] : "";
    p.value = argv[4] ? argv[4] : "";

    auto iter = std::find(((Type*)result)->defval.begin(), ((Type*)result)->defval.end(), p);
    if ((iter == ((Type*)result)->defval.end() || ((Type*)result)->defval.size() == 0))
        ((Type*)result)->defval.push_back(p);
    return 0;
}

// 
static int SetCallback(void* result, int argc, char** argv, char** azColName)
{
    Argument arg;
    arg.baseTypeName = argv[10] ? argv[10] : "";
    arg.kind = argv[9] ? argv[9] : "";
    arg.name = argv[5] ? argv[5] : "";
    arg.order = argv[4] ? atoi(argv[4]) : -1;
    arg.typeName = argv[8] ? argv[8] : "";

    auto iter = std::find(((Type*)result)->fields.begin(), ((Type*)result)->fields.end(), arg);
    if ((iter == ((Type*)result)->fields.end() || ((Type*)result)->fields.size() == 0) && arg.order != -1)
    {
        ((Type*)result)->fields.push_back(arg);
        ((Type*)result)->fields.sort();
    }
    return 0;
}

// функция выводящая дополнителбную информацию о типе
Type typeDisclosure(Argument arg)
{
    Type type;
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    std::string sql;

    /* Open database */
    rc = sqlite3_open("ApiMonitor.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return type;
    }
    
    if (arg.kind == "Struct" || arg.kind == "Union")
    {
        sql = "SELECT * FROM\
    (SELECT base_name, type_name AS tn, kind, fields_order, field_name, field_type_id FROM Types INNER JOIN Fields ON Fields.struct_type_id = Types.type_id) AS FT \
     INNER JOIN Types ON TYPES.type_id = FT.field_type_id\
     WHERE tn = \"" + arg.typeName + "\";";
        type = Type(arg.typeName, arg.baseTypeName, arg.kind);
        rc = sqlite3_exec(db, sql.c_str(), SetCallback, &type, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        
    }
    else if (arg.kind == "Alias")
    {
        type = Type(arg.typeName, arg.baseTypeName, arg.kind);
        sql = "SELECT type_name, kind, base_name, value_name, value FROM\
            Types INNER JOIN Def_Values ON Types.type_id = Def_Values.type_id\
            WHERE type_name =\"" + arg.typeName + "\";";
        rc = sqlite3_exec(db, sql.c_str(), DefValueCallback, &type, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    else
    {
        type = Type(arg.typeName, arg.baseTypeName, arg.kind);
    }
    sqlite3_close(db);
    return type;
}


/////////////////////////////////////////                                                               struct result


/////////////////////////////////////////                                                              DLLS, API, Return callback
static int DAR_callback(void* result, int argc, char** argv, char** azColName) {
    ((Function*)result)->DLL = argv[1] ? argv[1] : "";
    ((Function*)result)->API_Categorie = argv[2] ? argv[2] : "";
    ((Function*)result)->Return_Type = argv[3] ? argv[3] : "";
    return 0;
}

//                                                                                                         check name in database callback
static int Check_function_callback(void* result, int argc, char** argv, char** azColName)
{
   // for (int i = 0; i < argc; i++)
   //     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if (atoi(argv[0]) > 0)
        return 0;
    return 1;
}

///                                                                                                          argument callback
static int ARG_callback(void* result, int argc, char** argv, char** azColName) {
    Argument arg;
    arg.baseTypeName = argv[5] ? argv[5] : "";
    arg.kind = argv[4] ? argv[4] : "";
    arg.name = argv[2] ? argv[2] : "";
    arg.order = argv[1] ? atoi(argv[1]) : -1;
    arg.typeName = argv[3] ? argv[3] : "";

    auto iter = std::find(((Function*)result)->Arguments.begin(), ((Function*)result)->Arguments.end(), arg);
    if ((((Function*)result)->Arguments.end() == iter || ((Function*)result)->Arguments.size() == 0) && arg.order != -1)
    {
        ((Function*)result)->Arguments.push_back(arg);
        ((Function*)result)->Arguments.sort();
    }

    return 0;
}



/////////////////////////////////////////                                                               extract_from_db
Function extract_from_db(std::string name)
{
    Function function;
    function.Name = name;
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    std::string sql;

    /* Open database */
    rc = sqlite3_open("ApiMonitor.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(function);
    }
    //  check name in database
    sql = "SELECT count(*) FROM Functions WHERE func_name = \"" + name + "\";";
    rc = sqlite3_exec(db, sql.c_str(), Check_function_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return function;
    }

    /* Create SQL statement */ // extract arg. API. Return
    sql = "SELECT func_name, dll_name, api_name, type_name AS ret_type_name FROM \
        ((SELECT * FROM Functions LEFT JOIN API_Categories ON Functions.api_id = API_Categories.api_id) AS FA\
            LEFT JOIN DLLs ON DLLs.dll_id = FA.dll_id) AS FAD LEFT JOIN Types ON FAD.ret_type_id = Types.type_id\
        WHERE func_name = \"";
    sql += (name + "\";");

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), DAR_callback, &function, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    // extract argument
    sql = "SELECT func_name, arg_order, arg_name, type_name, kind, base_name FROM\
    (SELECT * FROM Functions LEFT JOIN Arguments ON Functions.func_id = Arguments.func_id) AS FA\
        LEFT JOIN Types ON Types.type_id = FA.arg_type_id\
        WHERE func_name = \"";
    sql += (name + "\";");
    rc = sqlite3_exec(db, sql.c_str(), ARG_callback, &function, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);
    return function;
}




/////////////////////////////////////////                                                               main

int main(int argc, char* argv[]) {
    Function function;
    std::string name = "getaddrinfo";
    function = extract_from_db(name);
    std::cout << function.API_Categorie << std::endl;
    std::cout << function.DLL << std::endl;
    std::cout << function.Name << std::endl;
    std::cout << function.Return_Type << std::endl;

    //std::cout << function.Arguments.front().order << std::endl;
    std::cout << "\n\n\n\n\n";
    for (auto i = function.Arguments.begin(); i != function.Arguments.end(); ++i)
    {
        std::cout << i->name << std::endl;
        Type q = typeDisclosure(*i);
        if (q.kind == "Alias")
        {
            for (auto j = q.defval.begin(); j != q.defval.end(); ++j)
            {
                std::cout << j->name << "    " << j->value << std::endl;
            }
        }
        std::cout << q.baseTypeName << std::endl;
        std::cout << q.kind << std::endl;
        std::cout << q.typeName << std::endl;
        std::cout << "\n\n" << std::endl;
    }

    return 0;
}
