#!/usr/bin/python

import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer
import random, os, datetime

address = "students.mimuw.edu.pl", 7553
log_dir = "/home/dokstud/lew/logi/%s" % datetime.datetime.now().isoformat()


report_file = "report.csv"
params_values = {"progressive_bias" : [100.0, 200.0], "explore_coeff" : [0.0, 0.1]}
params_names  = params_values.keys () #csv header

csv_header = ", ".join (params_names) + ", first_is_black, black_win, sgf_file"

params_of_series = []
games_per_series = 2

gnugo = "'gnugo-3.8 --mode gtp --chinese-rules --capture-all-dead --positional-superko --level=0'"
libego = "'/home/dokstud/lew/libego/bin/engine \"LoadGammas /home/dokstud/lew/libego/bin/3x3.gamma\" %s gtp'" 
gtp_set_command = "set"

# TODO seed, cputime

os.makedirs (log_dir)
os.chdir (log_dir)

def gen_params ():
    def select_param (x):
        vals = params_values[x]
        return x, vals [random.randrange (len (vals))]
    return dict ((select_param (x) for x in params_values))


def params_to_gtp (params):
    gtp = ""
    for param_name in params_names:
        gtp += '"%s %s %s"' % (gtp_set_command, param_name, params[param_name])
    return gtp


def format_report_line (series_id, first_is_black, black_win, sgf):
    params = params_of_series [series_id]
    csv_line = ", ".join ((params [param_name] for param_name in params_names))
    return scv_line + (', %d, %d, "%s"' % (first_is_black, black_win, sgf))


def get_game_setup ():
    global series_id
    params = gen_params ()
    params_of_series.append (params)
    series_id = len (params_of_series)
    ret = (log_dir,
           series_id,
           games_per_series,
           libego % params_to_gtp (params),
           gnugo
           )
    print len(ret)
    return ret


def report_game_result (series_id, result_list):
    f = open (report_file, "a")
    # TODO check for result length
    for (first_is_black, black_win, sgf) in result_list:
        f.write ("%s\n" % format_report_line (series_id, first_is_black, black_win, sgf))
    f.close ()
    return 0


server = SimpleXMLRPCServer (address)
server.register_function (get_game_setup, "get_game_setup")
server.register_function (report_game_result, "report_game_result")

print "Listening on port %d..." % address[1]

f = open (report_file, "a")
f.write ("%s\n" % csv_header)
f.close ()

server.serve_forever()
