#!/usr/bin/env python

import copy
import os
import re
import subprocess
import sys

re_func = re.compile('^  *[0-9]*: [0-9a-f]{16,16}  *'
                     '([0-9]*) FUNC    GLOBAL DEFAULT  *[0-9]* (.*)$')
re_header = re.compile('^ <([0-9a-f])><([0-9a-f]*)>: '
                       'Abbrev Number: [0-9]* \((DW_TAG_[_A-Za-z]*)\)$')
re_content = re.compile('^    <[0-9a-f]*>   (DW_AT_[_A-Za-z]*)  *: (.*)$')
re_name = re.compile('^\(indirect string, offset: 0x[0-9a-f]*\): (.*)$')
re_refid = re.compile('^<0x([0-9a-f]*)>$')
re_addr = re.compile('^  *[0-9]*: ([0-9a-f]{8,8})  *[0-9]* '
                     'FUNC    (GLOBAL|LOCAL)  *DEFAULT  *[0-9]* (.*)$')


class HexInt(int):
    pass


def get_debug_info_list(xft_path, host_readelf_path):
    debug_info_list = []
    for line in subprocess.check_output([host_readelf_path, '-W',
                                         '-wi', xft_path]).split('\n'):
        if re_header.match(line):
            header = re_header.sub(r'\1\0\2\0\3', line).split('\0')
            level = int(header[0])
            offset = int(header[1], 16)
            tag = header[2]
            debug_info_list += [{'level': level,
                                 'offset': offset, 'tag': tag, 'contents': {}}]
        elif re_content.match(line):
            content = re_content.sub(r'\1\0\2', line).split('\0')
            kind = content[0]
            value = content[1]
            debug_info_list[-1]['contents'][kind] = value
        else:
            pass
    return debug_info_list


def get_debug_info_dict(debug_info_list):
    debug_info_dict = {}
    for element in debug_info_list:
        debug_info_dict[element['offset']] = {'tag': element['tag'],
                                              'contents': element['contents']}
    return debug_info_dict


def get_debug_info_tree(debug_info_tree,
                        debug_info_list, current_level, current_element):
    while current_element != -1 and current_element < len(debug_info_list):
        if debug_info_list[current_element]['level'] > current_level:
            children = get_debug_info_tree([], debug_info_list,
                                           current_level + 1, current_element)
            debug_info_tree[-1]['children'], level, element = children
            current_element = element
        elif debug_info_list[current_element]['level'] < current_level:
            return debug_info_tree, current_level - 1, current_element
        else:
            debug_info_tree += [debug_info_list[current_element]]
            current_element += 1
    return debug_info_tree, -1, -1


def get_contents_recursively(prefix, debug_info_dict, type_offset):
    element = debug_info_dict[type_offset]
    if element['tag'] == 'DW_TAG_const_type':
        prefix += ['const']
    elif element['tag'] == 'DW_TAG_volatile_type':
        prefix += ['volatile']
    contents = copy.copy(element['contents'])
    if 'DW_AT_type' in contents:
        child = int(re_refid.sub(r'\1', contents['DW_AT_type']), 16)
        prefix, sub_contents = get_contents_recursively(prefix,
                                                        debug_info_dict, child)
        sub_contents.update(contents)
        contents = sub_contents
    return prefix, contents


def lookup_type(debug_info_dict, type_offset):
    tag = debug_info_dict[type_offset]['tag']
    prefix, element = get_contents_recursively([],
                                               debug_info_dict, type_offset)
    base_type = re_name.sub(r'\1', element.get('DW_AT_name', 'void'))
    if tag == "DW_TAG_pointer_type":
        type_name = (' '.join(prefix) + ' ' + base_type + ' *').strip()
        type_size = element['DW_AT_byte_size']
    else:
        type_name = (' '.join(prefix) + ' ' + base_type).strip()
        type_size = element['DW_AT_byte_size']
    return type_name, int(type_size)


def get_signature(debug_info_list, func):
    debug_info_dict = get_debug_info_dict(debug_info_list)
    tree, level, element = get_debug_info_tree([], debug_info_list, 0, 0)
    result = None
    for i in range(len(tree)):
        for element in tree[i]['children']:
            contents = element['contents']
            if re_name.sub(r'\1', contents.get('DW_AT_name', '')) == func:
                result = {}
                if 'children' not in element:
                    continue
                children = element['children']
                if 'DW_AT_type' in contents:
                    refid = re_refid.sub(r'\1', contents['DW_AT_type'])
                    arg = lookup_type(debug_info_dict, int(refid, 16))
                    result['return'] = {'type': arg[0], 'size': arg[1]}
                for child in children:
                    if child['tag'] != 'DW_TAG_formal_parameter':
                        continue
                    child_contents = child['contents']
                    refid = re_refid.sub(r'\1', child_contents['DW_AT_type'])
                    name = re_name.sub(r'\1', child_contents['DW_AT_name'])
                    arg = lookup_type(debug_info_dict, int(refid, 16))
                    elem = {name: {'type': arg[0], 'size': arg[1]}}
                    result['args'] = result.get('args', []) + [elem]
    return result


def get_global_funcs(xft_path, host_readelf_path):
    result = []
    for line in subprocess.check_output([host_readelf_path,
                                         '--dyn-syms', '--wide', xft_path]).split('\n'):
        if re_func.match(line):
            size, func_name = re_func.sub(r'\1\0\2', line).split('\0')
            if int(size) > 0:
                result += [func_name]
    return result


def get_func_addrs(global_funcs, axf_path, target_readelf_path):
    result = {}
    for line in subprocess.check_output([target_readelf_path,
                                         '-s', '-W', axf_path]).split('\n'):
        if re_addr.match(line):
            addr, symbol = re_addr.sub(r'\1\0\3', line).split('\0')
            if symbol in global_funcs:
                result[symbol] = result.get(symbol,
                                            []) + [HexInt(int(addr, 16))]
    return result


def get_formatted_xft_table(results):
    xft_table = []
    for key, value in sorted(results.items()):
        result = ['{}:\n'.format(key)]
        if 'return' in value:
            return_type = value['return']
            result += ['  return: {{type: {}, '.format(return_type['type'])
                       + 'size: {}}}\n'.format(return_type['size'])]
            comment = '#\n# {} {}'.format(return_type['type'], key)
        else:
            comment = '#\n# void {}'.format(key)
        if 'args' in value:
            result += ['  args:\n']
            comment += '('
            for arg in value['args']:
                name = arg.keys()[0]
                arg_type = arg[name]
                result += [
                    '  - {}: {{type: {}, '.format(name, arg_type['type'])
                    + 'size: {}}}\n'.format(arg_type['size'])]
                comment += '{} {}, '.format(arg_type['type'], name)
            comment = comment[:-2] + ')\n#   '
        else:
            comment += '(void)\n#   '
        result += ['  addrs:\n']
        for addr in value['addrs']:
            result += ['  - 0x{:08x}\n'.format(addr)]
            comment += '@0x{:08x}, '.format(addr)
        comment = comment[:-2] + '\n#\n'
        xft_table += [comment] + ['\n'] + result + ['\n']
    return xft_table


if __name__ == '__main__':
    host_readelf_path = os.getenv('HOST_READELF_PATH', '/usr/bin/readelf')
    target_readelf_path = os.getenv('TARGET_READELF_PATH',
                                    '/usr/bin/arm-none-eabi-readelf')
    if len(sys.argv) != 3:
        usage_string = 'usage: {} PATH_TO_XFT_LIBRARY PATH_TO_TARGET_BINARY\n'
        sys.stderr.write(usage_string.format(sys.argv[0]))
        sys.exit(os.EX_USAGE)
    global_funcs = get_global_funcs(sys.argv[1], host_readelf_path)
    func_addrs = get_func_addrs(global_funcs, sys.argv[2],
                                target_readelf_path=target_readelf_path)
    debug_info_list = get_debug_info_list(sys.argv[1],
                                          host_readelf_path=host_readelf_path)
    results = {}
    for symbol in global_funcs:
        results[symbol] = get_signature(debug_info_list, symbol)
        results[symbol].update({'addrs': func_addrs[symbol]})
    print(reduce(lambda x, y: x + y, get_formatted_xft_table(results)))
