#!/usr/bin/env python

import os
import re
import subprocess
import sys

re_symbol = re.compile(r'([_0-9a-z]{8}) <(.*)>:$')
re_line = re.compile(r'(.*[ ][ ]*)([0-9a-f][0-9a-f]*)(:.*)$')

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write('usage: {0} binary.elf\n'.format(sys.argv[0]))
        sys.exit(os.EX_USAGE)
    objdump = os.getenv('OBJDUMP')
    if objdump is None:
        objdump = 'objdump'
    try:
        result = subprocess.check_output(
            '{0} -z -w -S {1}'.format(objdump, sys.argv[1]), shell=True)
    except subprocess.CalledProcessError, e:
        sys.stderr.write('{0}\n'.format(e.output))
        sys.exit(os.EX_OSERR)

    addr_dict = {}
    for line in result.split('\n'):
        if re_symbol.match(line):
            symbol = re.sub(re_symbol, r'\2', line)
            base_addr = int(re.sub(re_symbol, r'\1', line), 16)
        elif re_line.match(line):
            addr = int(re.sub(re_line, r'\2', line), 16)
            addr_dict[addr] = '{0}+0x{1:x}'.format(symbol, addr - base_addr)

    for raw_line in sys.stdin:
        line = raw_line.strip('\n')
        if re_line.match(line):
            addr = int(re.sub(re_line, r'\2', line), 16)
            if addr in addr_dict:
                print '{0}{1:08x}{2:32}{3}'.format(
                    re.sub(re_line, r'\1', line),
                    addr, ' <' + addr_dict[addr] + '> ',
                    re.sub(re_line, r'\3', line))
            else:
                print '{0}{1:08x}{2:32}{3}'.format(
                    re.sub(re_line, r'\1', line),
                    addr, '', re.sub(re_line, r'\3', line))
        else:
            print line
