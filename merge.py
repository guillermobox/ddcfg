#!/usr/bin/env python2
# Merge all C files in a single one
#
import re

included_files = []

def include_file(filename):
    if filename in included_files:
        return ''

    included_files.append(filename)
    data = ''

    f = open(filename, 'r')
    for line in f.readlines():
        mat = re.match(r'#include "(.*)\.h"', line)
        if mat:
            filename = mat.groups()[0]
            data += include_file(filename + '.h')
            data += include_file(filename + '.c')
        else:
            data += line
    f.close()
    return data

print include_file("ddcfg.c")
