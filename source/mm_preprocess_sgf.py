#!/usr/bin/python

import sys
import re, os

file = ""
fail = False

def toCoords (m):
    if (m in ("tt", "ti")): return "pass"
    rows = "ABCDEFGHJKLMNOPQRST"
    row = ord(m[0]) - ord('a')
    col = 19-ord(m[1]) + ord('a')
    if (row in range(19) and col in range(1,20)):
        return "%s%d" % (rows [row], col)
    else:
        global fail
        fail = True

def doFile (name):
    global file, fail
    file = name
    fail = False

    f = open(name, 'rb').read()
    f = filter(lambda g: not g.isspace(), f)

    f = re.sub ('EV\[[^]]*\]', '', f)
    f = re.sub ('PW\[[^]]*\]', '', f)
    f = re.sub ('PB\[[^]]*\]', '', f)
    f = re.sub ('SO\[[^]]*\]', '', f)
    f = re.sub ('DT\[[^]]*\]', '', f)

    assert (f[0] == '(' and f[-1] == ')')
    f = f[1:-1]
    l = f.split (';');

    assert (l[0] == "")

    header = l[1]
    moves = l[2:]

    size = re.search('SZ\\[([^]]*)\\]', header).group(1)

    ret_moves = []


    handi = re.search('AB((\[..\])+)', header)
    if handi != None:
        hcs = handi.group(1).split("[")
        assert (hcs [0] == "")
        hcs = hcs[1:]
        handi_n = 0
        for hc in hcs:
            assert (len(hc) == 3)
            assert (hc[2] == ']')
            ret_moves.append ("B %s" % toCoords (hc[:2]))
            handi_n += 1
        xx = re.search('HA\[([1-9])\]', header)
        if xx != None: 
            assert handi_n == int (xx.group(1))
            


    for m in moves:
        if not len (m) in (3, 5):
            fail = True
            return (19, [])
        assert (m[0] in ('W', 'B'))
        assert (m[1] == '[')
        assert (m[-1] == ']')
        if (len(m) == 3): 
            ret_moves.append ("%s pass" % (m[0]))
        else:
            ret_moves.append ("%s %s" % (m[0], toCoords (m[2:4])))

    return (size, ret_moves)


try:
    for root, _, files in os.walk(sys.argv[1], topdown=False):
        for name in files:
            (size, moves) = doFile ((os.path.join(root, name)))
            if (not fail):
                print "file", file
                print size, len(moves)
                print "\n".join (moves)
                print
            else:
                sys.stderr.write ("Bad file: ")
                sys.stderr.write (file)
                sys.stderr.write ("\n")
except Exception, err:
    sys.stderr.write (file + "\n")
    raise err
