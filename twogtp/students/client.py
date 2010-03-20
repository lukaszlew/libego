#!/usr/bin/python

import xmlrpclib, time, sys, subprocess, os, re, sys

address = "students.mimuw.edu.pl", 7553
proxy = xmlrpclib.ServerProxy("http://%s:%d/" % address)
gnugo = "'/home/dokstud/lew/.bin/gnugo-3.8 --mode gtp --chinese-rules --capture-all-dead --positional-superko --level=0'"

def do_one_series ():
    task = proxy.get_game_setup ()
    (log_dir, series_id, game_count, first, second) = task
    if series_id == -1: 
        print "No more tasks, exiting."
        return False

    print "New task. Series %d, %d games." %(series_id, game_count)
    print "First : ", first
    print "Second: ", second

    os.chdir (log_dir)

    sgf_prefix = "game-%d" % series_id
    args = [
        "/home/dokstud/lew/.bin/gogui-twogtp",
        "-auto",
        "-size", "9",
        "-komi", "6.5",
        "-alternate",
        "-referee", gnugo,
        "-games", "%d" % game_count,
        "-black", first,
        "-white", second,
        "-sgffile", sgf_prefix,
        # "-verbose",
        ]

    cmd = " ".join (args)
    process = subprocess.Popen (cmd,
                                stdout = subprocess.PIPE,
                                stderr = subprocess.STDOUT,
                                shell = True)
    sys.stdout.write ("Waiting for results...")
    twogtp_log = process.communicate () [0]
    sys.stdout.write ("\r\n")

    print twogtp_log

    result_list = []
    for game_no in range (game_count):
        file_name = sgf_prefix + "-" + str (game_no) + ".sgf"
        sgf_file = open (file_name, "r")
        sgf = sgf_file.read ()
        sgf_file.close ()
        winner_letter = re.search (r'RE\[([^]]*)\]', sgf).group(1) [0]
        assert (winner_letter in 'BW')
        result = (game_no % 2 == 0, 1 if winner_letter == 'B' else 0, file_name)
        print "%10s | Winner: %s | First is %s" % (file_name,
                                                   winner_letter,
                                                   "black" if game_no % 2 == 0 else "white"
                                                   )
        result_list.append (result)

    print "Results: ", result_list, "\n"
    proxy.report_game_result (series_id, result_list)
    return True

while do_one_series ():
    pass
