#!/usr/bin/python

import xmlrpclib, time, sys, subprocess, os

address = "students.mimuw.edu.pl", 7553
proxy = xmlrpclib.ServerProxy("http://%s:%d/" % address)



def do_one_series ():
    log_dir, series_id, game_count, black, white = proxy.get_game_setup ()
    os.chdir (log_dir)

    args = [
        "gogui-twogtp",
        "-auto",
        "-size", "9",
        "-komi", "6.5",
        "-alternate",
        "-referee", gnugo,
        "-games", "%d" % game_count,
        "-black", black,
        "-white", white,
        "-sgffile", "%d" % series_id,
        "-verbose",
        ]

    process = subprocess.Popen (" ".join (args),
                                stdout = subprocess.PIPE,
                                stderr = subprocess.STDOUT,
                                shell = True)
    twogtp_log = process.communicate () [0]

    print twogtp_log
    print

    result_list = [(1, 1, "file1.sgf"), (0,0, "file2.sgf")]
    # TODO parse twogtp_log
    proxy.report_game_result (series_id, result_list)

do_one_series ()
