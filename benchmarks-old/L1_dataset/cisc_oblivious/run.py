#!/usr/bin/env python3

###################################################################
#
#   Arguments:  argv[1]: benchmark name(e.g. dijkstra)
#               argv[2:]: arguments
#
######################################################################

import os
import sys


BENCHMARK = sys.argv[1]
EXECUTABLE = BENCHMARK + ".o"


m2s_dict = {    "binary_search" : "m2s_1L1",        \
                "find_max"      : "m2s_original",   \
                "int_sort"      : "m2s_1L1",        \
                "kmeans"        : "m2s_1L1",        \
                "matrix_mult"   : "m2s_original",   \
                "page_rank"     : "m2s_1L1",        \
                "oram"          : "m2s_2L1",        \
                "dnn"           : "m2s_original",   \
                "PQ"            : "m2s_1L1",        \
                "dijkstra"      : "m2s_4L1"         }

mem_config_dict = { "binary_search" : "mem_config_1L1.ini",        \
                    "find_max"      : "mem_config_original.ini",   \
                    "int_sort"      : "mem_config_1L1.ini",        \
                    "kmeans"        : "mem_config_1L1.ini",        \
                    "matrix_mult"   : "mem_config_original.ini",   \
                    "page_rank"     : "mem_config_1L1.ini",        \
                    "oram"          : "mem_config_2L1.ini",        \
                    "dnn"           : "mem_config_original.ini",   \
                    "PQ"            : "mem_config_1L1.ini",        \
                    "dijkstra"      : "mem_config_4L1.ini"         }

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

MULTI2SIM_PATH = "./" + m2s_dict[BENCHMARK]
MEM_CONFIG_FILE = mem_config_dict[BENCHMARK]
os.system(MULTI2SIM_PATH + " --x86-config x86_config.ini --mem-config " + MEM_CONFIG_FILE + " --x86-sim detailed " + EXECUTABLE)




