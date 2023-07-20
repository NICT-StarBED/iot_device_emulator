#!/usr/bin/env python

import os
import re
import sys
import yaml


def get_type_string(description, types):
    if description['type'][-1] == '*':
        length = 4
    else:
        length = (description['size'] + 3) / 4 * 4
    types.add(length)
    return 'size_of_{}_t'.format(length), length


def generate_reg_name():
    for n in range(4):
        yield 'context->registers.r[R{}]'.format(n)
    n = 0
    while True:
        n += 1
        template = '*(word *)&context->memory.memory[{}]'
        yield template.format('context->registers.r[SP] - {}'.format(n * 4))


def _create_glue(info, xft_path_base):
    struct_lengths = set([])
    func_pointers = []
    for function in sorted(info.keys()):
        if 'args' in info[function]:
            argument_list = ''
            for arg in info[function]['args']:
                type_string, length = get_type_string(arg[arg.keys()[0]],
                                                      struct_lengths)
                argument_list += '{} {}, '.format(type_string, arg.keys()[0],
                                                  struct_lengths)
            argument_list = argument_list[:-2]
        else:
            argument_list = 'void'
        if 'return' in info[function]:
            type_string, length = get_type_string(arg[arg.keys()[0]],
                                                  struct_lengths)
            func_pointers += ['{} (*{})({});'.format(type_string,
                                                     function, argument_list)]
        else:
            func_pointers += ['{} (*{})({});'.format('void',
                                                     function, argument_list)]
    output = [
        '#include <sys/types.h>',
        '#include <dlfcn.h>',
        '#include <stdio.h>',
        '#include <stdlib.h>',
        '#include "e4k64f_impl.h"',
        ''
    ]
    for length in struct_lengths:
        template = 'typedef struct {{u_int32_t word[{}];}} size_of_{}_t;'
        output += [template.format(length / 4, length)]
    output += ['']
    for function in sorted(info.keys()):
        template = 'static void dispatch_{}(e4k64f_context_t *context);'
        output += [template.format(function)]
    output += [
        '',
        'void *{}_handle;'.format(re.sub('\.', '_', xft_path_base)),
        '']
    for func_pointer in func_pointers:
        output += ['{}'.format(func_pointer)]
    output += [
        '',
        'u_int8_t **xft_memory_offset;',
        '',
        'int xft_setup(e4k64f_context_t *context)',
        '{']
    template = '    if(({}_handle = dlopen("{}", RTLD_NOW)) == NULL) {{'
    output += [
        template.format(re.sub('\.', '_', xft_path_base), xft_path_base),
        '        perror("dlopen");',
        '        exit(EXIT_FAILURE);',
        '    }']
    template = ''.join(['    if((xft_memory_offset = dlsym(',
                        '{}_handle, "xft_memory_offset")) == NULL) {{'])
    output += [
        template.format(re.sub('\.', '_', xft_path_base), xft_path_base),
        '        perror("dlsym");',
        '        exit(EXIT_FAILURE);',
        '    }',
        '    *xft_memory_offset = (u_int8_t *)context->memory.memory;']
    for function in sorted(info.keys()):
        template = '    if(({} = dlsym({}_handle, "{}")) == NULL) {{'
        output += [
            template.format(function,
                            re.sub('\.', '_', xft_path_base), function),
            '        perror("dlsym");',
            '        exit(EXIT_FAILURE);',
            '    }',
        ]
    output += [
        '    return 0;',
        '}',
        '',
        'int xft_teardown(void)',
        '{',
        '    dlclose({}_handle);'.format(re.sub('\.', '_', xft_path_base)),
        '    return 0;',
        '}',
        '',
        'int xft_dispatch(word addr, e4k64f_context_t *context)',
        '{',
        '    int result = -1;',
        '',
        '    switch(addr & 0xfffffffe) {',
    ]
    for function in sorted(info.keys()):
        for addr in info[function]['addrs']:
            output += ['    case 0x{:08x}:'.format(addr & 0xfffffffe)]
        output += [
            '        dispatch_{}(context);'.format(function),
            '        result = 1;',
            '        break;',
        ]
    output += [
        '    default:',
        '        result = 0;',
        '        break;',
        '    }',
        '    return result;',
        '}',
    ]
    for function in sorted(info.keys()):
        template = 'static void dispatch_{}(e4k64f_context_t *context)'
        output += [
            '',
            template.format(function),
            '{']
        if 'args' in info[function]:
            args = generate_reg_name()
            for arg in info[function]['args']:
                contents = ''
                type_string, length = get_type_string(arg[arg.keys()[0]],
                                                      struct_lengths)
                for i in range(length / 4):
                    contents += '{}, '.format(args.next())
                contents = contents[:-2]
                template = '    {} {} = {{{}}};'
                output += [template.format(type_string,
                                           arg.keys()[0], contents)]
        if 'return' in info[function]:
            type_string, length = get_type_string(info[function]['return'],
                                                  struct_lengths)
            output += [
                '    {} result;'.format(type_string)
            ]
        output += [
            '']
        argument_list = ''
        if 'args' in info[function]:
            for arg in info[function]['args']:
                argument_list += '{}, '.format(arg.keys()[0])
            argument_list = argument_list[:-2]
        if 'return' in info[function]:
            type_string, length = get_type_string(info[function]['return'],
                                                  struct_lengths)
            output += [
                '    result = {}({});'.format(function, argument_list)]
            args = generate_reg_name()
            for i in range(length / 4):
                output += [
                    '    {} = result.word[{}];'.format(args.next(), i)]
        else:
            output += [
                '    {}({});'.format(function, argument_list)]
        output += [
            '}']
    return '\n'.join(output)


def main(input, xft_path):
    info = yaml.load(input)
    print(_create_glue(info, os.path.basename(xft_path)))


if __name__ == '__main__':
    if len(sys.argv) != 3:
        usage_string = 'usage: {} PATH_TO_XFT_LIBRARY\n'
        sys.stderr.write(usage_string.format(sys.argv[0]))
        sys.exit(os.EX_USAGE)
    if sys.argv[1] == '-':
        with sys.stdin as f:
            main(f, sys.argv[2])
    else:
        with open(sys.argv[1]) as f:
            main(f, sys.argv[2])
