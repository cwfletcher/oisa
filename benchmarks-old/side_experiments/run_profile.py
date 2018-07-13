#!/usr/bin/env python

###################################################################
#
#   Arguments:  argv[1]: benchmark name(e.g. dijkstra)
#               argv[2:]: arguments
#
######################################################################

import os
import sys
import numpy
import contextlib
import multiprocessing
from collections import deque
from datetime import datetime


CUR_DIR = os.getcwd() + "/"
RESULT_DIR = CUR_DIR + "result/"

BENCHMARK = sys.argv[1]
EXECUTABLE = BENCHMARK + ".o"


m2s_dict = {    "oram_stash_in_1L2" : "m2s_1L2",        \
                "path_oram"         : "m2s_original",   \
                "PLS_1L1"           : "m2s_1L1",        \
                "PLS_2L1"           : "m2s_2L1",        \
                "PLS_4L1"           : "m2s_4L1",   \
                "PLS_1L2"           : "m2s_1L2",        \
                "PLS_2L2"           : "m2s_2L2",        \
                "scan"              : "m2s_original"   }

mem_config_dict = { "oram_stash_in_1L2" : "mem_config_1L2.ini",        \
                    "path_oram"         : "mem_config_original.ini",   \
                    "PLS_1L1"           : "mem_config_1L1.ini",        \
                    "PLS_2L1"           : "mem_config_2L1.ini",        \
                    "PLS_4L1"           : "mem_config_4L1.ini",   \
                    "PLS_1L2"           : "mem_config_1L2.ini",        \
                    "PLS_2L2"           : "mem_config_2L2.ini",        \
                    "scan"              : "mem_config_original.ini" }

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



def executeProgram(param):
    MULTI2SIM_PATH = "./" + m2s_dict[BENCHMARK]
    MEM_CONFIG_FILE = mem_config_dict[BENCHMARK]

    global TRACE_PACKAGE
    global TRACE_FILE
    global DISASM_FILE
    global EXE_INSTR_FILE

    TRACE_PACKAGE   = RESULT_DIR + BENCHMARK + param.replace(" ", "_") + "_trace.gz"
    TRACE_FILE      = RESULT_DIR + BENCHMARK + param.replace(" ", "_") + "_trace"
    DISASM_FILE     = RESULT_DIR + BENCHMARK + param.replace(" ", "_") + "_disasm"
    EXE_INSTR_FILE  = RESULT_DIR + BENCHMARK + param.replace(" ", "_") + "_debug_isa.no_sim_flag"

    print ("=== Run ", BENCHMARK, " with param ", param, "at", str(datetime.now()), " ===")

    print (" == Generate disassembly at ", str(datetime.now()), "== ")
    ## Generate disassembly
    os.system("rm " + DISASM_FILE)
    os.system(MULTI2SIM_PATH + " --x86-disasm " + EXECUTABLE + " > " + DISASM_FILE)

    print (" == Generate debug isa file at ", str(datetime.now()), "== ")
    ## Generate debug isa file
    os.system("rm " + EXE_INSTR_FILE)
    os.system(MULTI2SIM_PATH + " --x86-config x86_config.ini --mem-config " + MEM_CONFIG_FILE + " --x86-debug-isa " + EXE_INSTR_FILE + " " + EXECUTABLE + " " + param)

    print (" == Generate trace package at ", str(datetime.now()), "== ")
    ## Generate trace file
    os.system("rm " + TRACE_PACKAGE)
    os.system(MULTI2SIM_PATH + " --x86-config x86_config.ini --mem-config " + MEM_CONFIG_FILE + " --x86-sim detailed --trace " + TRACE_PACKAGE + " " + EXECUTABLE + " " + param)

    print (" == Generate trace file at ", str(datetime.now()), "== ")
    ## Uncompress the generated .gz trace package
    os.system("rm " + TRACE_FILE)
    os.system("gunzip " + TRACE_PACKAGE)


def calcCommitTime(param):

    print ("=== Calculate commit time at ", str(datetime.now()), " ===")

    ## Record all starting address of all function from Disassembly file
    func_list = []
    entrance_func_map = {}
    with open(DISASM_FILE) as disasm:
        for line in disasm:
            if '<' in line and '>' in line:
                func = line[line.find('<')+1 : line.find('>')]
                if func == "main" or func[0].isupper():
                    func_list.append(func)
                    entrance = int(line.split()[0], 16)
                    entrance_func_map[entrance] = func

    ## Find all call/ret instruction from debug-isa file
    committed_instrs = []
    num_calls = 0
    num_rets  = 0
    record_target_addr = False
    record_ret_addr = False
    with open(EXE_INSTR_FILE) as exe_instr_file:
        for line in exe_instr_file:
            instr = ((line[line.find(':')+1:]).split('(')[0]).strip()
            addr = int((line.split()[2])[:-1], 16)
            if record_target_addr:
                record_target_addr = False
                committed_instrs[-1][-2] = addr
            if record_ret_addr:
                record_ret_addr = False
                committed_instrs[-1][-1] = addr
            if "call" in instr:
                num_calls = num_calls + 1
                record_target_addr = True
                if "DWORD" in instr:
                    ret_addr = addr + int((line.split('(')[1]).split()[0])
                    committed_instrs.append([addr, instr, 0, "DEADBEEF", ret_addr])
                else:
                    ret_addr = addr + int((line.split('(')[1]).split()[0])
                    committed_instrs.append([addr, instr, 0, "DEADBEEF", ret_addr])
            if "ret" in instr:
                num_rets  = num_rets + 1
                record_ret_addr = True
                committed_instrs.append([addr, instr, 0, 0, "DEADBEEF"])

    ## Find committed timestamp from trace file
    trace_buffer = deque([])
    clock = 0
    num_committed_instr = 0
    with open(TRACE_FILE) as trace_file:
        for line in trace_file:
            if line[0] == 'c':
                clock = int(line.split('=')[1])
            else:
                if "\"co\"" in line: # find this committed instruction in trace_buffer
                    instr_id = int((line.split('=')[1]).split()[0])
                    while len(trace_buffer) > 0:
                        oldest_instr = trace_buffer.popleft()
                        if oldest_instr[0] > instr_id:
                            trace_buffer.appendleft(oldest_instr)
                            break
                        elif oldest_instr[0] == instr_id:
                            assert committed_instrs[num_committed_instr][1] == oldest_instr[1]
                            committed_instrs[num_committed_instr][2] = oldest_instr[2]
                            num_committed_instr = num_committed_instr + 1
                            break
                else:
                    if " asm" in line:
                        instr_id = int((line.split('=')[1]).split()[0])
                        instr    = line[line.index("asm")+5:line.index("uasm")-2]
                        if "call" in instr or "ret" in instr:
                            trace_buffer.append([instr_id, instr, clock])


    ## Search for all port addresses, build the transit stack
    transit_stack = []
    call_stack = [["_start", []]]

    for instr_info in committed_instrs:
        addr, instr, clock, target, ret_addr = instr_info
        if "call" in instr:
            # print (hex(addr), "call: jump to ", hex(target))
            if target != ret_addr:
                call_stack[-1][1].append(ret_addr)
                if target in entrance_func_map:
                    transit_stack.append([call_stack[-1][0], entrance_func_map[target], clock])
                    call_stack.append([entrance_func_map[target], []])

        elif "ret" in instr:
            # print (hex(addr), "ret: jump to ", hex(ret_addr))
            if len(call_stack[-1][1]) == 0:
                if call_stack[-2][1][-1] == ret_addr:
                    transit_stack.append([call_stack[-1][0], call_stack[-2][0], clock])
                    call_stack.pop()
                    call_stack[-1][1].pop()
                # else:
                    # print ("false ret! jump to ", hex(ret_addr))
            else:
                if call_stack[-1][1][-1] == ret_addr:
                    call_stack[-1][1].pop()
                # else:
                    # print ("false ret! jump to ", hex(ret_addr))

        else:
            assert 0, "current instruction is %s, which is not call or ret" %(instr)



    # assert call_stack[0][0] == "before_all" and len(call_stack) == 1, "call stack is: %s" %(str(call_stack))

    ## Use the transit stack to compute the time spent on each function
    func_time = {}
    for func in func_list:
        func_time[func] = 0

    func_time["_start"] = transit_stack[0][2]
    for i in range(len(transit_stack) - 1):
        curr_tx = transit_stack[i]
        next_tx = transit_stack[i+1]
        # print(curr_tx, next_tx)
        assert curr_tx[1] == next_tx[0], "FUCK!!!"
        func_time[curr_tx[1]] = func_time[curr_tx[1]] +  int(next_tx[2]) - int(curr_tx[2])

    total_cycles = 0
    for func, time in func_time.items():
        total_cycles = total_cycles + time

    profile_name = RESULT_DIR + "output_" + BENCHMARK + param.replace(" ", "_") + ".txt"
    with open(profile_name, "w") as output:
        for func, time in sorted(func_time.items(), key=lambda x: x[1], reverse=True):
            percent = float(time) / total_cycles * 100
            output.write(func + " : " + str(percent) + "% = " + str(time) + " cycles\n")
        for transit in transit_stack:
            output.write(str(transit) + '\n')

        output.write("======== end =========\n")


######################################################################
##
##      Put your test code here
##
######################################################################
N = sys.argv[2]
B = sys.argv[3]
executeProgram(N + " " + B)
calcCommitTime(N + " " + B)


