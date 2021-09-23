import sqlite3

with sqlite3.connect("ApiMonitor.db") as connection:
    cursor = connection.cursor()

    # Создаём таблицу API_Categories
    cursor.execute(
        '''CREATE TABLE API_Categories(
            api_id INTEGER PRIMARY KEY NOT NULL,
            api_name TEXT UNIQUE NOT NULL
            )
        ''')

    # Создаём таблицу DLLs
    cursor.execute(
        '''CREATE TABLE DLLs(
            dll_id INTEGER PRIMARY KEY NOT NULL,
            dll_name TEXT UNIQUE NOT NULL
            )
        ''')

    # Создаём таблицу Types
    cursor.execute(
        '''CREATE TABLE Types(
            type_id INTEGER PRIMARY KEY NOT NULL,
            type_name TEXT UNIQUE NOT NULL,
            kind TEXT NOT NULL,
            base_name TEXT
            )
        ''')

    # Создаём таблицу Functions
    cursor.execute(
        '''CREATE TABLE Functions(
            func_id INTEGER PRIMARY KEY NOT NULL,
            func_name TEXT NOT NULL,
            api_id INTEGER,
            dll_id INTEGER,
            ret_type_id INTEGER NOT NULL,
            FOREIGN KEY (api_id) REFERENCES API_Categories(api_id),
            FOREIGN KEY (dll_id) REFERENCES DLLs(dll_id),
            FOREIGN KEY (ret_type_id) REFERENCES Types(type_id)
            )
        ''')

    # Создаём таблицу Arguments
    cursor.execute(
        '''CREATE TABLE Arguments(
            arg_id INTEGER PRIMARY KEY NOT NULL,
            arg_name TEXT NOT NULL,
            arg_order INTEGER NOT NULL,
            func_id INTEGER NOT NULL,
            arg_type_id INTEGER NOT NULL,
            FOREIGN KEY (func_id) REFERENCES Functions(func_id),
            FOREIGN KEY (arg_type_id) REFERENCES Types(type_id)
            )
        ''')

    # Создаём таблицу Def_Values
    cursor.execute(
        '''CREATE TABLE Def_Values(
            value_id INTEGER PRIMARY KEY NOT NULL,
            value_name TEXT NOT NULL,
            value BLOB NOT NULL,
            type_id INTEGER NOT NULL,
            FOREIGN KEY (type_id) REFERENCES Types(type_id)
            )
        ''')

    # Создаём таблицу Fields
    cursor.execute(
        '''CREATE TABLE Fields(
            field_id INTEGER PRIMARY KEY NOT NULL,
            field_name TEXT NOT NULL,
            fields_order INTEGER NOT NULL,
            struct_type_id INTEGER NOT NULL,
            field_type_id INTEGER NOT NULL,
            FOREIGN KEY (struct_type_id) REFERENCES Types(type_id),
            FOREIGN KEY (field_type_id) REFERENCES Types(type_id)
            )
        ''')