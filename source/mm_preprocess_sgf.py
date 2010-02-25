#!/usr/bin/python

import sys
import re, os

def doFile (name):
    f = open(name, 'rb').read()
    f = filter(lambda g: not g.isspace(), f)

    assert (f[0] == '(' and f[-1] == ')')
    f = f[1:-1]
    l = f.split (';');

    assert (l[0] == "")

    header = l[1]
    moves = l[2:]

    size = re.search('SZ\\[([^]]*)\\]', header).group(1)
    komi = re.search('KM\\[([^]]*)\\]', header).group(1)

    rows = "ABCDEFGHJKLMNOPQRST"
    
    print size
    print komi


    for m in moves:
        assert (len (m) in (3, 5))
        assert (m[0] in ('W', 'B'))
        assert (m[1] == '[')
        assert (m[-1] == ']')
        if (len(m) == 3): 
            print "%s pass" % (m[0])
        else:
            print "%s %s%s" % (m[0], rows [ord(m[2]) - ord('a')], 19-ord(m[3]) + ord('a'))
    print


for root, _, files in os.walk(sys.argv[1], topdown=False):
    for name in files:
        doFile ((os.path.join(root, name)))
