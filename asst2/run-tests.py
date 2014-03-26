#
# The run script I use to do all of the mpi runs
#

import re
import sys
import os
import json
from subprocess import check_output 

class MPIRun:

    MAX_RUNS  = 5
    HOST_FILE = "../machine-assignments.txt"
    NUM_SLOTS = 2

    def __init__(self, a, b, n, p):
        self.a = a
        self.b = b
        self.n = n
        self.p = p
        self.failed     = 0
        self.speedup    = 1
        self.efficiency = 1
        self.results = []
        self.hosts   = []
        
        # create the re pattern
        self._create_pattern()
        self._get_hosts()
    
    def run(self):
        
        i = 0
        while i < MPIRun.MAX_RUNS:
            run = self._mpirun()

            if not run:
                continue

            self.results.append(run)
            i += 1
        
        # sort the results
        self.results = sorted(self.results, cmp=lambda a, b: cmp(a['elapsed_time'], b['elapsed_time']))
    
    def output(self, f):
    
        f.write("a = %f, b = %f, n = %d, p = %d\n" % (self.a, self.b, self.n, self.p))
        f.write("speedup = %f, efficiency = %f\n\n" % (self.speedup, self.efficiency))
        f.write(json.dumps(self.results, indent=4))
        f.write("\n\n")
    
    def calc_speedup(self, run):
        self.speedup    = run.results[0]['elapsed_time'] / self.results[0]['elapsed_time']
        self.efficiency = self.speedup / self.p

    def _mpirun(self):
        hostfile = self._create_host_file()
        
        cmd = 'echo "%d %d %d" | mpirun --prefix /opt/openmpi -hostfile %s ./bin/integrate' % \
            (self.a, self.b, self.n, hostfile)
        
        try:
            output = check_output(cmd, shell=True)
        except:
            return None

        return self._parse_output(output)
    
    def _parse_output(self, output):
        m = self.re.search(output)

        if not m:
            return {
                "num_processors"  : 0,
                "elapsed_time"    : 0,
                "num_trapezoids"  : 0,
                "a"               : 0,
                "b"               : 0,
                "result"          : 0,
                "abs_rel_t_error" : 0,
                "min_error"       : 0,
            }

        data = {
            "num_proccessors" : int(m.group(1)),
            "elapsed_time"    : float(m.group(2)),
            "num_trapezoids"  : int(m.group(4)),
            "a"               : float(m.group(5)),
            "b"               : float(m.group(6)),
            "result"          : float(m.group(7)),
            "abs_rel_t_error" : float(m.group(9)),
            "min_error"       : float(m.group(11)),
        }
        return data
        
    
    def _create_pattern(self):
        float_e_pattern     = "(\d+.\d+e(-|\+)?\d+)"
        end_of_line_pattern = ".+\s+"

        output_re_pattern = \
            r"Running on (\d+) processor%(eol)s" + \
            r"Elapsed time = %(float)s %(eol)s" + \
            r"With n = (\d+)%(eol)s" + \
            r"our estimate.+from (\d+\.\d+) to (\d+\.\d+) = %(float)s%(eol)s" + \
            r"absolute .+ = %(float)s .+ %(float)s"

        output_re_pattern = output_re_pattern % \
                            {"float": float_e_pattern, "eol": end_of_line_pattern}

        self.re = re.compile(output_re_pattern, re.M);
    
    def _get_hosts(self):        
        f = open(MPIRun.HOST_FILE, "r")
                
        for line in f:
            self.hosts.append(line.strip())
            
        self.hosts.sort()
        
        f.close()
        
    def _create_host_file(self):
        
        # the hosts file depends on the number of processes
        file_name = "%d-hosts.txt" % (self.p)
        
        if os.path.exists(file_name):
            return file_name
        
        # create the file
        f = open(file_name, "w")
        
        num_hosts = int(self.p / MPIRun.NUM_SLOTS) # two processes per host
        
        i = 0
        while i < num_hosts:
            host = self.hosts[i]
            f.write("%s slots=%d\n" % (host, MPIRun.NUM_SLOTS))
            i += 1

        slots = self.p % MPIRun.NUM_SLOTS

        if slots > 0:
            f.write("%s slots=%d\n" % (self.hosts[i], slots))
        
        f.close()
        
        return file_name

out_f = open("run-output.txt", 'w')

base_n = 1090313
base_n = 1100000

n_vals = [
    base_n,
    base_n * 2,
    base_n * 4,
    base_n * 8,
    base_n / 2,
    base_n / 4,
    base_n / 8,
]
p_vals = [
    1, 2, 4, 8, 14, 16, 20,
]

for n in n_vals:
    output = "\n\n======== n = %d =========\n\n" % n
    
    print output,
    out_f.write(output)

    mpiruns = []
    for p in p_vals:
        print "p = %d" % p
        r = MPIRun(100, 600, n, p)
        mpiruns.append(r)

        r.run()

        if r.failed:
            print "run failed"
            continue

        r.calc_speedup(mpiruns[0])
        r.output(out_f)

out_f.close()
