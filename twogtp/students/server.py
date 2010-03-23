#!/usr/bin/python

import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer
import random, os, datetime, sys, traceback

if len (sys.argv) != 2:
    print "Usage: %s <time_in_minutes>" % sys.argv [0]
    sys.exit (1)

uptime_minutes = float (sys.argv[1])

start_time = datetime.datetime.now()
log_dir = "/home/dokstud/lew/logi/%s" % start_time.isoformat()

address = "students.mimuw.edu.pl", 7553


report_file = "report.csv"

#TODO list of strings
# params_values = {
#     "progressive_bias" : [25.0, 35.0, 50.0, 70.0, 100.0, 140.0, 200.0, 280.0, 400.0],
#     "progressive_bias_prior" : [-200.0, -50.0, -10.0, 0.0, 10.0, 50.0, 200.0],
#     }

params_values = {
    "progressive_bias" : [100.0],
    "progressive_bias_prior" : [0.0],
    }

params_names  = params_values.keys () #csv header

csv_header = ", ".join (params_names) + ", first_is_black, black_win, sgf_file"

params_of_series = []
games_per_series = 2

gnugo = \
    "'/home/dokstud/lew/.bin/gnugo-3.8 --mode gtp --chinese-rules" \
    " --capture-all-dead --positional-superko --level=0'"

libego = \
    "'/home/dokstud/lew/libego/bin/engine" \
    " \"LoadGammas /home/dokstud/lew/libego/bin/3x3.gamma\"" \
    " %s gtp'" 

gtp_set_command = "set"

# TODO seed, cputime
workers = {}
idle_workers = {}

def gen_params ():
    def select_param (x):
        vals = params_values[x]
        return x, vals [random.randrange (len (vals))]
    return dict ((select_param (x) for x in params_values))


def params_to_gtp (params):
    gtp = ""
    for param_name in params_names:
        gtp += '"%s %s %f" ' % (gtp_set_command, param_name, params[param_name])
    return gtp


def format_report_line (series_id, first_is_black, black_win, sgf):
    params = params_of_series [series_id]
    csv_line = ", ".join ((str(params [param_name]) for param_name in params_names))
    # scv_line, oh yeah :)
    return csv_line + (', %d, %d, "%s"' % (first_is_black, black_win, sgf))


def report_idling (worker_id):
    idle_workers [worker_id] = datetime.datetime.now()
    if (worker_id in workers):
        print "Warning! Idling worker on worker list."
    return 0
    

def get_game_setup (worker_id):
    if (worker_id in workers):
        print "Warning! Worker already on worker list."
    if (worker_id in idle_workers):
        idle_workers.pop (worker_id)
    workers [worker_id] = datetime.datetime.now()

    global series_id
    params = gen_params ()
    series_id = len (params_of_series)
    params_of_series.append (params)
    ret = (log_dir,
           series_id,
           games_per_series,
           libego % params_to_gtp (params),
           gnugo
           )
    return ret


def report_game_result (worker_id, clog_dir, series_id, result_list):
    # error handling
    if (not worker_id in workers):
        print "Old report from ", worker_id
        if (clog_dir == log_dir):
            print "Warning! but the same experiment dir!"
        return 0;
    if clog_dir != log_dir: 
        print "Warning! in pool, but wrong dir!!"
        return 0

    worker_start = workers.pop (worker_id)
    f = open (report_file, "a")
    # TODO check for result length
    for (first_is_black, black_win, sgf) in result_list:
        print (first_is_black, black_win, sgf)
        f.write ("%s\n" % format_report_line (series_id, first_is_black, black_win, sgf))
    f.close ()
    return 0


def list_workers ():
    s = "ACTIVE:\n"
    for w in workers:
        s += "%s %s\n" % (w, workers[w])
    s += "IDLE:\n"
    for w in idle_workers:
        s += "%s %s\n" % (w, idle_workers[w])
    return s

os.makedirs (log_dir)
os.chdir (log_dir)

server = SimpleXMLRPCServer (address)

server.register_function (get_game_setup, "get_game_setup")
server.register_function (report_game_result, "report_game_result")
server.register_function (list_workers, "list_workers")
server.register_function (report_idling, "report_idling")

print "Listening on port %d..." % address[1]

f = open (report_file, "a")
f.write ("%s\n" % csv_header)
f.close ()

server.serve_forever()
