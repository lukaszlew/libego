#!/usr/bin/python

import xmlrpclib, time, sys, subprocess, os, re, sys, socket, multiprocessing

address = "students.mimuw.edu.pl", 7553
proxy = xmlrpclib.ServerProxy("http://%s:%d/" % address)
print proxy.list_workers ()
