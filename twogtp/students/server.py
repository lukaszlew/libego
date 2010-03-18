#!/usr/bin/python

import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer
import random, os, datetime

address = "students.mimuw.edu.pl", 7553
log_dir = "/home/dokstud/lew/logi/%s" % datetime.datetime.now().isoformat()
report_file = "report.csv"
params_of_series = []
games_per_series = 2

gnugo = "'gnugo-3.8 --mode gtp --chinese-rules --capture-all-dead --positional-superko --level=0'"
libego = "'/home/dokstud/lew/libego/bin/engine \"LoadGammas /home/dokstud/lew/libego/bin/3x3.gamma\" %s gtp'" 

# TODO seed, cputime

os.makedirs (log_dir)
os.chdir (log_dir)

def gen_params ():
    pass

def params_to_gtp (params):
    return ""

def format_report_header ():
    return "as_black, black_win, sgf_file"

def format_report_line (series_id, as_black, result, sgf):
    return "%d, %d, %s" % (as_black, result, sgf)

def get_game_setup ():
    global series_id
    params = gen_params ()
    params_of_series.append (params)
    return (log_dir,
            len (params_of_series),
            games_per_series,
            libego % params_to_gtp (params),
            gnugo
            )


def report_game_result (series_id, result_list):
    f = open (report_file, "a")
    # TODO check for result length
    for (as_black, result, sgf) in result_list:
        f.write ("%s\n" % format_report_line (series_id, as_black, result, sgf))
    f.close ()
    return 0


server = SimpleXMLRPCServer (address)
server.register_function (get_game_setup, "get_game_setup")
server.register_function (report_game_result, "report_game_result")

print "Listening on port %d..." % address[1]
server.serve_forever()
