import sqlite3
import xml_reader

path_to_directory = 'API'
path_to_database = 'ApiMonitor.db'
with sqlite3.connect(path_to_database) as connection:
    cursor = connection.cursor()
    functions, types = xml_reader.parse_all_file(path_to_directory, [])

    # Заполняем таблицу Types значениями из словаря types
    types_list = []
    types_query = '''INSERT OR IGNORE INTO Types (type_name, kind, base_name) VALUES (?,?,?);'''
    for type in types:
        type_name = type['type_name']
        kind = type['kind']
        base_name = type['base_name']
        if not base_name:
            base_name = None
        types_list.append((type_name, kind, base_name))
    cursor.executemany(types_query, types_list)
    connection.commit()

    # Заполняем таблицы Def_Values и Fields значениями из словаря types
    values_list = []
    values_query = '''INSERT OR IGNORE INTO Def_Values (value_name, value, type_id) VALUES (?,?,?);'''
    fields_list = []
    fields_query = '''INSERT OR IGNORE INTO Fields (field_name, fields_order, struct_type_id, field_type_id) VALUES (?,?,?,?);'''
    new_types_list = []
    type_search_query = '''SELECT type_id FROM Types WHERE type_name = ?'''
    for type in types:
        # Заполняем поле type_id в таблицах Def_Values и Fields
        cursor.execute(type_search_query, (type['type_name'],))
        try:
            type_id = cursor.fetchone()[0]
        except TypeError:
            type_id = None

        # Заполняем остальные поля таблицы Def_Values
        def_values = type['def_values']
        if def_values:
            for def_value in def_values:
                flag_name = def_value[0]
                flag_value = def_value[1].encode()
                values_list.append((flag_name, flag_value, type_id))

        # Заполняем остальные поля таблицы Fields
        fields = type['fields']
        if fields:
            for order_number, field in enumerate(fields):
                field_type = field[0]
                field_name = field[1]
                if not field_name:
                    field_name = 'NOT INDICATED'
                field_type_id = None
                cursor.execute(type_search_query, (field_type,))
                try:
                    field_type_id = cursor.fetchone()[0]
                except TypeError:
                    while field_type[-1] == '*':
                        new_types_list.append((field_type, 'Pointer', field_type[:-1]))
                        field_type = field_type[:-1]
                    cursor.executemany(types_query, new_types_list)
                    connection.commit()
                    cursor.execute(type_search_query, (field_type,))
                    try:
                        field_type_id = cursor.fetchone()[0]
                    except TypeError:
                        cursor.execute(types_query, (field_type, 'Unknown', None))
                        connection.commit()
                        cursor.execute(type_search_query, (field_type,))
                        field_type_id = cursor.fetchone()[0]
                fields_list.append((field_name, order_number, type_id, field_type_id))
    cursor.executemany(values_query, values_list)
    cursor.executemany(fields_query, fields_list)
    connection.commit()

    # Заполняем таблицы API_Categories и DLLs значениями из словаря functions
    api_names_list = []
    api_categories_query = '''INSERT OR IGNORE INTO API_Categories (api_name) VALUES (?);'''
    dll_names_list = []
    dlls_query = '''INSERT OR IGNORE INTO DLLs (dll_name) VALUES (?);'''
    for function in functions:
        dll_name = function['dll_name']
        if dll_name == 'none' or dll_name == '*':
            continue
        dll_names_list.append((dll_name,))
        api_name = function['category']
        if api_name == 'none':
            continue
        api_names_list.append((api_name,))
    cursor.executemany(api_categories_query, api_names_list)
    cursor.executemany(dlls_query, dll_names_list)
    connection.commit()

    # Заполняем таблицу Functions значениями из словаря functions
    functions_list = []
    functions_query = '''INSERT INTO Functions (func_name, api_id, dll_id, ret_type_id) VALUES (?,?,?,?);'''
    new_types_list.clear()
    for function in functions:
        # Заполняем поле func_name таблицы Functions
        func_name = function['func_name']

        # Заполняем поле api_id таблицы Functions
        api_search_query = '''SELECT api_id FROM API_Categories WHERE api_name = ?'''
        cursor.execute(api_search_query, (function['category'],))
        try:
            api_id = cursor.fetchone()[0]
        except TypeError:
            api_id = None

        # Заполняем поле dll_id таблицы Functions
        dll_search_query = '''SELECT dll_id FROM DLLs WHERE dll_name = ?'''
        cursor.execute(dll_search_query, (function['dll_name'],))
        try:
            dll_id = cursor.fetchone()[0]
        except TypeError:
            dll_id = None

        # Заполняем поле ret_type_id таблицы Functions
        ret_value_type = function['ret_value_type']
        cursor.execute(type_search_query, (ret_value_type,))
        try:
            ret_type_id = cursor.fetchone()[0]
        except TypeError:
            while ret_value_type[-1] == '*':
                new_types_list.append((ret_value_type, 'Pointer', ret_value_type[:-1]))
                ret_value_type = ret_value_type[:-1]
            cursor.executemany(types_query, new_types_list)
            connection.commit()
            cursor.execute(type_search_query, (ret_value_type,))
            try:
                ret_type_id = cursor.fetchone()[0]
            except TypeError:
                cursor.execute(types_query, (ret_value_type, 'Unknown', None))
                connection.commit()
                cursor.execute(type_search_query, (ret_value_type,))
                ret_type_id = cursor.fetchone()[0]
        functions_list.append((func_name, api_id, dll_id, ret_type_id))
    cursor.executemany(functions_query, functions_list)
    connection.commit()

    # Заполняем таблицу Arguments значениями из словаря functions
    arguments_list = []
    arguments_query = '''INSERT OR IGNORE INTO Arguments (arg_name, arg_order, func_id, arg_type_id) VALUES (?,?,?,?);'''
    new_types_list.clear()
    for function in functions:
        # Заполняем поле func_id таблицы Arguments
        func_search_query = '''SELECT func_id FROM Functions WHERE func_name = ?'''
        cursor.execute(func_search_query, (function['func_name'],))
        try:
            func_id = cursor.fetchone()[0]
        except TypeError:
            func_id = None

        # Заполняем остальные поля таблицы Arguments
        arguments = function['params']
        if arguments:
            for order_number, argument in enumerate(arguments):
                arg_type = argument[0]
                arg_name = argument[1]
                arg_type_id = None
                cursor.execute(type_search_query, (arg_type,))
                try:
                    arg_type_id = cursor.fetchone()[0]
                except TypeError:
                    while arg_type[-1] == '*':
                        new_types_list.append((arg_type, 'Pointer', arg_type[:-1]))
                        arg_type = arg_type[:-1]
                    cursor.executemany(types_query, new_types_list)
                    connection.commit()
                    cursor.execute(type_search_query, (arg_type,))
                    try:
                        arg_type_id = cursor.fetchone()[0]
                    except TypeError:
                        cursor.execute(types_query, (arg_type, 'Unknown', None))
                        connection.commit()
                        cursor.execute(type_search_query, (arg_type,))
                        arg_type_id = cursor.fetchone()[0]
                arguments_list.append((arg_name, order_number, func_id, arg_type_id))
    cursor.executemany(arguments_query, arguments_list)