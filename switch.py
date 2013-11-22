#!/usr/bin/env python2
#
import sys
import os
import decimal
import itertools
import copy
from ConfigParser import SafeConfigParser

METHOD = 'CROSS'
USAGE = '''
Usage: {0} <inputfile> <parameter> <range> [<parameter> <range>]'

Where:
    inputfile:  Name of the file to use as imput
    parameter:  Name of the parameter to vary
    range:      Expression for the new value of the parameter
'''
WARNING = '''
*** WARNING ***
The Python parser is not fully compatible with the ini parser used
in the C library. The most notable difference is that the ini parser
allows to have options without a section. This is impossible in ConfigParser
so in this program every option without section will be classified as [root].
*** WARNING ***
'''

class FakeSecHead(object):
    def __init__(self, fp):
        self.fp = fp
        self.sechead = '[root]\n'
    def readline(self):
        if self.sechead:
            try:
                return self.sechead
            finally:
                self.sechead = None
        else:
            return self.fp.readline()

class SpecialParser(SafeConfigParser):
    def readfile(self, filename):
        self.filename = filename
        self.filehandler = open(self.filename, 'r')
        self.readfp(FakeSecHead(self.filehandler))
    def resetdefault(self):
        self.readfile(self.filename)
    def optionxform(self, option):
        return option

def usage():
    print USAGE.format(sys.argv[0])

def drange(start, stop, step):
    r = start
    while r <= stop:
        yield '%g'%float(r)
        r += step

def expand_int(field):
    tk = field.split('-')
    return range(int(tk[0]), int(tk[1])+1)

def expand_float(field):
    tk = field.split(':')
    if len(tk) == 2:
        return drange(\
                decimal.Decimal(tk[0]),
                decimal.Decimal(tk[1]),
                decimal.Decimal(1.0))
    else:
        return drange(\
                decimal.Decimal(tk[0]),
                decimal.Decimal(tk[2]),
                decimal.Decimal(tk[1]))

def expand_range(s):
    '''
    Take a range as input, and create a list of values for
    that range.
    '''
    s = "".join(s.split())
    ret = list()
    for field in s.split(','):
        if ':' in field:
            ret.extend(expand_float(field))
        elif '-' in field:
            ret.extend(expand_int(field))
        else:
            ret.append(field)
    return ret

def parse_argv(argv):
    filename = None
    switches = []
    if argv == []:
        return filename, switches

    filename = argv.pop(0)
    while argv != []:
        if len(argv)<2:
            sys.stderr.write("\nError! Impossible to parse command line, parameter {0} does not have range\n".format(argv[0]))
            usage()
            exit(1)
        switches.append((argv.pop(0), argv.pop(0)))
    return filename, switches

def check_switches(parsed, switches):
    for switch in switches:
        key = switch[0]
        if '.' in key:
            section, _, option = key.partition('.')
        else:
            section, option = 'root', key
        if not section in parsed.sections():
            return key
        if not option in parsed.options(section):
            return key
    return None

def create_scenarios(switches, method=METHOD):
    keys = []
    values = []
    for s in switches:
        keys.append(s[0])
        values.append(expand_range(s[1]))
    return keys, [pair for pair in itertools.product(*values)]

def show_scenarios(switches, values):
    print
    print 'The following changes will be applied to the configuration file:'
    print
    for switch in switches:
        print '%20s:'%switch[0], ', '.join(map(str,expand_range(switch[1])))
    print
    print 'Using method {0} for a total of: {1} files'.format(METHOD, len(values))
    print
    input = raw_input('Do you want to continue? ')
    if input.lower() in ['yes', 'y', '']:
        return True
    else:
        return False

def apply_scenarios(filename, parsed, keys, values):
    counter = 0
    if len(values)<10:
        fmt = '{0}.{1}'
    elif len(values)<100:
        fmt = '{0}.{1:02}'
    else:
        fmt = '{0}.{1:03}'

    for pair in values:
        counter += 1
        newfile = fmt.format(filename, counter)
        print 'Writing', newfile, '...',

        for key, val in zip(keys, pair):
            sec, _, opt = key.partition('.')
            parsed.set(sec, opt, str(val))

        parsed.write(open(newfile,'w'))
        parsed.resetdefault()
        print 'ok'

def main():
    if len(sys.argv) < 3:
        usage()
        exit(1)

    print WARNING

    filename, switches = parse_argv(sys.argv[1:])

    parsed = SpecialParser()
    parsed.readfile(filename)

    err = check_switches(parsed, switches)
    if err:
        print 'Section/Option not found in the file:', err
        exit(1)

    keys, values = create_scenarios(switches)
    if show_scenarios(switches, values):
        apply_scenarios(filename, parsed, keys, values)
    else:
        print 'Ok, aborting'

if __name__ == '__main__':
    main()
