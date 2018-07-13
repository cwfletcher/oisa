#!/usr/bin/env python3

###################################################################
#
#   Arguments:  argv[1]: benchmark name(e.g. dijkstra)
#               argv[2:]: arguments
#
######################################################################

import os
import sys


MULTI2SIM_PATH = "./m2s_original"

BENCHMARK = sys.argv[1]
EXECUTABLE = BENCHMARK + ".o"


class Unbuffered(object):
    def __init__(self, stream):
        self.stream = stream
    def write(self, data):
        self.stream.write(data)
        self.stream.flush()
    def writelines(self, data):
        self.stream.writelines(data)
        self.stream.flush()
    def __getattr__(self, attr):
        return getattr(self.stream, attr)


sys.stdout = Unbuffered(sys.stdout)

os.system(MULTI2SIM_PATH + " --x86-config x86_config.ini --mem-config mem_config_original.ini --x86-sim detailed " + EXECUTABLE)




