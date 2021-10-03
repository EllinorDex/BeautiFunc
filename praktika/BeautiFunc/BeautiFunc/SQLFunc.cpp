#include "SQLFunc.h"
#include "sqlite/sqlite3.h"
#include <string>
#include "Function.h"
#include <stdlib.h>
#include "Type.h"
#include <algorithm>
#include "Extract.h"

static int DefValueCallback(void* result, int argc, char** argv, char** azColName)
{
    pair p;
    p.name = argv[3] ? argv[3] : "";
    p.value = argv[4] ? argv[4] : "";
    std::list<pair> temp = ((Type*)result)->GetDefVal();
    auto iter = std::find(temp.begin(), temp.end(), p);
    if ((iter == temp.end() || temp.size() == 0))
        ((Type*)result)->PushDefVal(p);
    return 0;
}

static int SetCallback(void* result, int argc, char** argv, char** azColName)
{
    Argument arg;
    arg.SetBaseTypeName(argv[10] ? argv[10] : "");
    arg.SetKind(argv[9] ? argv[9] : "");
    arg.SetName(argv[5] ? argv[5] : "");
    arg.SetOrder(argv[4] ? atoi(argv[4]) : -1);
    arg.SetTypeName(argv[8] ? argv[8] : "");

    std::list<Argument> temp = ((Type*)result)->GetFields();
    auto iter = std::find(temp.begin(), temp.end(), arg);
    if ((iter == temp.end() || temp.size() == 0) && arg.GetOrder() != -1)
    {
        ((Type*)result)->PushField(arg);
        ((Type*)result)->SortFields();
    }
    return 0;
}

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
    type.SetBaseTypeName(arg.GetBaseTypeName());
    type.SetKind(arg.GetKind());
    type.SetTypeName(arg.GetTypeName());

    if (arg.GetKind() == "Struct" || arg.GetKind() == "Union")
    {
        sql = "SELECT * FROM\
    (SELECT base_name, type_name AS tn, kind, fields_order, field_name, field_type_id FROM Types INNER JOIN Fields ON Fields.struct_type_id = Types.type_id) AS FT \
     INNER JOIN Types ON TYPES.type_id = FT.field_type_id\
     WHERE tn = \"" + arg.GetTypeName() + "\";";

        rc = sqlite3_exec(db, sql.c_str(), SetCallback, &type, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

    }
    else if (arg.GetKind() == "Alias")
    {
        sql = "SELECT type_name, kind, base_name, value_name, value FROM\
            Types INNER JOIN Def_Values ON Types.type_id = Def_Values.type_id\
            WHERE type_name =\"" + arg.GetTypeName() + "\";";
        rc = sqlite3_exec(db, sql.c_str(), DefValueCallback, &type, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    sqlite3_close(db);
    return type;
}


/////////////////////////////////////////                                                               struct result


/////////////////////////////////////////                                                              DLLS, API, Return callback
static int DAR_callback(void* result, int argc, char** argv, char** azColName) {
    ((Function*)result)->Set_DLL(argv[1] ? argv[1] : "");
    ((Function*)result)->Set_API_Categorie(argv[2] ? argv[2] : "");
    ((Function*)result)->Set_Return_Value(argv[3] ? argv[3] : "");
    return 0;
}

//                                                                                                         check name in database callback
static int Check_function_callback(void* result, int argc, char** argv, char** azColName)
{
    if (atoi(argv[0]) > 0)
        return 0;
    return 1;
}

///                                                                                                          argument callback
static int ARG_callback(void* result, int argc, char** argv, char** azColName) {
    Argument arg;
    arg.SetBaseTypeName(argv[5] ? argv[5] : "");
    arg.SetKind(argv[4] ? argv[4] : "");
    arg.SetName(argv[2] ? argv[2] : "");
    arg.SetOrder(argv[1] ? atoi(argv[1]) : -1);
    arg.SetTypeName(argv[3] ? argv[3] : "");

    std::list<Argument> temp = ((Function*)result)->Get_Arguments();
    auto iter = std::find(temp.begin(), temp.end(), arg);
    if ((temp.end() == iter || temp.size() == 0) && arg.GetOrder() != -1)
    {
        ((Function*)result)->PushArg(arg);
        ((Function*)result)->SortArg();
    }

    return 0;
}



/////////////////////////////////////////                                                               extract_from_db
Function extract_from_db(std::string name)
{
    Function function;
    function.Set_Name(name);
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

static int BaseNameCallback(void* result, int argc, char** argv, char** azColName)
{
    ((Arg*)result)->kind = argv[1] ? argv[1] : "";
    ((Arg*)result)->baseTypeName = argv[2] ? argv[2] : "";
    return 0;
}

static int UnCallback(void* result, int argc, char** argv, char** azColName)
{
    Arg field = typeInformation(argv[7] ? argv[7] : "");
    field.name = argv[4] ? argv[4] : "";
    field.order = argv[3] ? atoi(argv[3]) : -1;
 

    if (field.order != -1)
    {
        ((Arg*)result)->Fields.push_back(field);
    }
    return 0;
}

static int AliasCallback(void* result, int argc, char** argv, char** azColName)
{
    DefVal p;
    p.name = argv[3] ? argv[3] : "";
    p.value = argv[4] ? argv[4] : "";
    ((Arg*)result)->defVal.push_back(p); 
    return 0;
}

Arg typeInformation(std::string typeName)
{
    Arg type;
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    std::string sql;

    rc = sqlite3_open("ApiMonitor.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return type;
    }
    type.typeName = typeName;
    sql = "SELECT type_name, kind, base_name FROM Types\
     WHERE type_name = \"" + typeName + "\";";
    rc = sqlite3_exec(db, sql.c_str(), BaseNameCallback, &type, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }


    if (type.baseTypeName != "")
    {
        type.baseType = new Arg(typeInformation(type.baseTypeName));
    }
    else
        type.baseType = NULL;


    if (type.kind == "Struct" || type.kind == "Union")
    {
        sql = "SELECT * FROM\
    (SELECT base_name, type_name AS tn, kind, fields_order, field_name, field_type_id FROM Types INNER JOIN Fields ON Fields.struct_type_id = Types.type_id) AS FT \
     INNER JOIN Types ON TYPES.type_id = FT.field_type_id\
     WHERE tn = \"" + typeName + "\";";

        rc = sqlite3_exec(db, sql.c_str(), UnCallback, &type, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

    }
    else if (type.kind == "Alias")
    {
        sql = "SELECT type_name, kind, base_name, value_name, value FROM\
            Types INNER JOIN Def_Values ON Types.type_id = Def_Values.type_id\
            WHERE type_name =\"" + typeName + "\";";
        rc = sqlite3_exec(db, sql.c_str(), AliasCallback, &type, &zErrMsg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    sqlite3_close(db);
    return type;
}