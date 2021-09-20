import xml.etree.ElementTree as xml
import copy
import os

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
    for i in parse_all_file('Api',[])[1]:
        print(i)

    print(len(parse_all_file('Api',[])[1]))
    # dir_path = 'Api'
    # stop_dir = [
    #     'Headers',
    # ]

    # parse_all_file(dir_path)
    # for i in parse(file_path):
    #     print(i)