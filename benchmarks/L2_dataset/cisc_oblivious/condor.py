#!/usr/bin/env python

import os

os.system("make clean")
os.system("make sim")
os.system("rm ./result/*")
os.system("rm ./condor_out/*")
os.system("condor_submit condor_jobs")
