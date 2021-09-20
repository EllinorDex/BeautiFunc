import os
import copy
import sqlite3
import xml.etree.ElementTree as xml

func_info_q = {
    'func_name': '',
    'dll_name': '',
    'category': '',
    'ret_value_type': '',
    'params': []
}

type_info_q = {
    'type_name': '',
    'kind': '',
    'base_name': '',
    'fields': [],
    'def_values': []
}


def parse_variable(variable):
    type_info = copy.deepcopy(type_info_q)
    type_info['type_name'] = variable.attrib['Name']
    type_info['kind'] = variable.attrib['Type']

    if 'Base' in variable.attrib:
        type_info['base_name'] = variable.attrib['Base']

    if variable.find('Enum') is not None:
        enum = variable.find('Enum')
        for set in enum:
            type_info['def_values'].append((set.attrib['Name'], set.attrib['Value']))
    elif variable.find('Flag') is not None:
        flag = variable.find('Flag')
        for set in flag:
            type_info['def_values'].append((set.attrib['Name'], set.attrib['Value']))
    elif variable.findall('Field') is not None:
        fields = variable.findall('Field')
        for field in fields:
            type_info['fields'].append((field.attrib['Type'], field.attrib['Name']))

    return type_info


def parse_file(file_path):
    func_info_arr = []
    type_info_arr = []
    tree = xml.parse(file_path)
    root = tree.getroot()
    wrapper = root.find('Module')
    class_name = ''
    if wrapper is not None:
        dll_name = wrapper.attrib['Name']
        category = 'none'
    elif root.find('Interface') is not None:
        dll_name = 'none'
        wrapper = root.find('Interface')
        class_name = wrapper.attrib['Name']
        category = wrapper.attrib['Category']
    elif root.find('Headers') is not None:
        wrapper = root.find('Headers')
        cond_flag = False
        for variable in wrapper:
            if variable.tag == 'Condition' and cond_flag == False:
                cond_flag = True
                for inner_variable in variable:
                    type_info = parse_variable(inner_variable)
                    type_info_arr.append(type_info)

            elif variable.tag == 'Condition' and cond_flag == True:
                continue
            else:
                type_info = parse_variable(variable)
                type_info_arr.append(type_info)

        return func_info_arr, type_info_arr

    else:
        return func_info_arr, type_info_arr

    for elem in wrapper:
        if elem.tag == 'Api':
            func_info = copy.deepcopy(func_info_q)
            func_info['func_name'] = elem.attrib['Name'] if class_name == '' else class_name + '.' + elem.attrib['Name']
            func_info['dll_name'] = dll_name
            func_info['category'] = category
            func_info['ret_value_type'] = elem.find('Return').attrib['Type']
            for arg in elem.findall('Param'):
                func_info['params'].append((arg.attrib['Type'], arg.attrib['Name']))
            func_info_arr.append(func_info)
        elif elem.tag == 'Category':
            category = elem.attrib['Name']
        elif elem.tag == 'Variable':
            type_info = parse_variable(elem)
            type_info_arr.append(type_info)

    return func_info_arr, type_info_arr


def parse_all_file(dir_path, stop_dir):
    bd_func = []
    bd_type = []
    for root, dirs, files in os.walk(dir_path):
        if (set(stop_dir) & set(root.split('\\'))):
            continue
        for file in files:
            bd = parse_file(root + '\\' + file)
            bd_func = bd_func + bd[0]
            bd_type = bd_type + bd[1]

    return bd_func, bd_type

if __name__ == '__main__':
    path_to_directory = 'D:\\Desktop\\practice\\API\\API'
    bd_func, bd_type = parse_all_file(path_to_directory, [])
    connection = sqlite3.connect("ApiMonitor.db")
    cursor = connection.cursor()
    types_list = []
    types_query = '''INSERT OR IGNORE INTO Types (type_name, kind, base_name) VALUES (?,?,?);'''
    fields_list = []
    fields_query = '''INSERT OR IGNORE INTO Fields (field_name, fields_order, struct_type_id, field_type_id) VALUES (?,?,?,?);'''
    values_list = []
    values_query = '''INSERT OR IGNORE INTO Def_Values (value_name, value, type_id) VALUES (?,?,?);'''
    for type in bd_type:
        type_name = type['type_name']
        if type_name == 'ICreateDeviceAccessAsync**':
            print('gh')
        kind = type['kind']
        base_name = type['base_name']
        if base_name == '':
            base_name = 'NULL'
        types_list.append((type_name, kind, base_name))
        type_search_query = '''SELECT type_id FROM Types WHERE type_name = ?'''
        cursor.execute(type_search_query, (type_name,))
        try:
            type_id = cursor.fetchone()[0]
        except TypeError:
            type_id = 'NULL'
        fields = type['fields']
        if fields:
            for order_number, field in enumerate(fields):
                field_type = field[0]
                field_name = field[1]
                field_search_query = '''SELECT type_id FROM Types WHERE type_name = ?'''
                cursor.execute(field_search_query, (field_type,))
                try:
                    field_type_id = cursor.fetchone()[0]
                except TypeError:
                    field_type_id = 'NULL'
                fields_list.append((field_name, order_number, type_id, field_type_id))
        def_values = type['def_values']
        if def_values:
            for def_value in def_values:
                flag_name = def_value[0]
                flag_value = def_value[1]
                values_list.append((flag_name, flag_value.encode(), type_id))
    cursor.executemany(types_query, types_list)
    connection.commit()
    cursor.executemany(fields_query, fields_list)
    connection.commit()
    cursor.executemany(values_query, values_list)
    dll_names_list = []
    dlls_query = '''INSERT OR IGNORE INTO DLLs (dll_name) VALUES (?);'''
    api_names_list = []
    api_categories_query = '''INSERT OR IGNORE INTO API_Categories (api_name) VALUES (?);'''
    functions_list = []
    functions_query = '''INSERT INTO Functions (func_name, api_id, dll_id, ret_type_id) VALUES (?,?,?,?);'''
    arguments_list = []
    arguments_query = '''INSERT OR IGNORE INTO Arguments (arg_name, arg_order, func_id, arg_type_id) VALUES (?,?,?,?);'''
    new_types_list = []
    for func in bd_func:
        dll_name = func['dll_name']
        if dll_name == 'none' or dll_name == '*':
            continue
        dll_names_list.append((dll_name,))
        api_name = func['category']
        if api_name == 'none':
            continue
        api_names_list.append((api_name,))
        func_name = func['func_name']
        api_search_query = '''SELECT api_id FROM API_Categories WHERE api_name = ?'''
        cursor.execute(api_search_query, (api_name,))
        try:
            api_id = cursor.fetchone()[0]
        except TypeError:
            api_id = 'NULL'
        dll_search_query = '''SELECT dll_id FROM DLLs WHERE dll_name = ?'''
        cursor.execute(dll_search_query, (dll_name,))
        try:
            dll_id = cursor.fetchone()[0]
        except TypeError:
            dll_id = 'NULL'
        type_search_query = '''SELECT type_id FROM Types WHERE type_name = ?'''
        cursor.execute(type_search_query, (func['ret_value_type'],))
        try:
            ret_type_id = cursor.fetchone()[0]
        except TypeError:
            ret_type_id = 'NULL'
        functions_list.append((func_name, api_id, dll_id, ret_type_id))
        func_search_query = '''SELECT func_id FROM Functions WHERE func_name = ?'''
        cursor.execute(func_search_query, (func_name,))
        try:
            func_id = cursor.fetchone()[0]
        except TypeError:
            func_id = 'NULL'
        arguments = func['params']
        if arguments:
            for order_number, argument in enumerate(arguments):
                arg_type = argument[0]
                arg_name = argument[1]
                argument_search_query = '''SELECT type_id FROM Types WHERE type_name = ?'''
                cursor.execute(argument_search_query, (arg_type,))
                try:
                    arg_type_id = cursor.fetchone()[0]
                except TypeError:
                    while arg_type[-1] == '*':
                        new_types_list.append((arg_type, 'Pointer', arg_type[:-1]))
                        arg_type = arg_type[:-1]
                    cursor.execute(argument_search_query, (arg_type,))
                    try:
                        cursor.fetchone()[0]
                    except TypeError:
                        new_types_list.append((arg_type, 'Interface', 'NULL'))
                arguments_list.append((arg_name, order_number, func_id, arg_type_id))
    cursor.executemany(types_query, new_types_list)
    connection.commit()
    cursor.executemany(api_categories_query, api_names_list)
    connection.commit()
    cursor.executemany(dlls_query, dll_names_list)
    connection.commit()
    cursor.executemany(functions_query, functions_list)
    connection.commit()
    cursor.executemany(arguments_query, arguments_list)
    connection.commit()
    connection.close()