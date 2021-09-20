#include <iostream>
#include "sqlite/sqlite3.h"
#include <string>
#include "Function.h"
#include <stdlib.h>


/////////////////////////////////////////                                                               struct result


/////////////////////////////////////////                                                              DLLS callback
static int DAR_callback(void* result, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++)
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    ((Function*)result)->DLL = argv[1] ? argv[1] : "";
    ((Function*)result)->API_Categorie = argv[2] ? argv[2] : "";
    ((Function*)result)->Return_Type = argv[3] ? argv[3] : "";
    return 0;
}
static int Check_function_callback(void* result, int argc, char** argv, char** azColName)
{
    for (int i = 0; i < argc; i++)
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if (atoi(argv[0]) > 0)
        return 0;
    return 1;
}

static int ARG_callback(void* result, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++)
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    Argument arg;
    arg.baseTypeName = argv[5] ? argv[5] : "";
    arg.kind = argv[4] ? argv[4] : "";
    arg.name = argv[2] ? argv[2] : "";
    arg.order = argv[1] ? atoi(argv[1]) : -1;
    arg.typeName = argv[3] ? argv[3] : "";
    ((Function*)result)->Arguments.push_back(arg);
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
    else {
        fprintf(stderr, "Opened database successfully\n");
    }

    sql = "SELECT count(*) FROM Functions WHERE func_name = \"" + name + "\";";
    rc = sqlite3_exec(db, sql.c_str(), Check_function_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return function;
    }
    else {
        fprintf(stdout, "Operation done successfully\n");

    }

    /* Create SQL statement */
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
    else {
        fprintf(stdout, "Operation done successfully\n");
    }
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
    else {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return function;
}




/////////////////////////////////////////                                                               main

int main(int argc, char* argv[]) {
    Function function;
    std::string name = "DebugCreate";
    function = extract_from_db(name);
    std::cout << function.API_Categorie << std::endl;
    //std::cout << function.Arguments.front().order << std::endl;
    return 0;
}
