import re
import sys
import os
import json
from subprocess import Popen, PIPE

class MPIRun:

    MAX_RUNS  = 5
    HOST_FILE = "../machine-assignments.txt"
    NUM_SLOTS = 2

    def __init__(self, a, b, n, p):
        self.a = a
        self.b = b
        self.n = n
        self.p = p
        self.speedup    = 1
        self.efficiency = 1
        self.results = []
        
        # create the re pattern
        self._create_pattern()
        self._get_hosts()
    
    def run(self):
        
        i = 0
        while i < MPIRun.MAX_RUNS:
            self.results.append(self._mpirun())
            i++
        
        # sort the results
        self.results = sorted(self.results, cmp=lambda a, b: compare(a.elapsed_time, b.elapsed_time))
    
    def output(self, f):
    
        f.write("a = %f, b = %f, n = %d, p = %d\n" % (self.a, self.b, self.n, self.p))
        f.write("speedup = %f, efficiency = %f\n\n" % (self.speedup, self.efficiency))
        f.write(json.dumps(self.results, indent=4))
        f.write("\n\n")
    
    def calc_speedup(self, run)
        self.speedup    = run.results[0]['elapsed_time'] / self.results[0]['elapsed_time']
        self.efficiency = self.speedup / self.p
    
    def _mpirun(self):
        hostfile = self._create_host_file()
        
        inp_args = ['echo', '"%d %d %d"' % (self.a, self.b, self.n)]
        mpi_args = ['mpirun', '--prefix', '/opt/openmpi', '-hostfile', hostfile, "./bin/integrate"]
        
        p1 = Popen(inp_args, stdout=PIPE)
        p2 = Popen(mpi_args, stdin=p1.stdout, stdout=PIPE)
        p1.stdout.close()
        output = p2.communicate()[0]
        
        return self._parse_output(output)
        
    
    def _parse_output(self, output):
        m = self.re.search(output)
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
            r"Running on (\d+) processors%(eol)s" + \
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
            i++
        f.close()
        
        return file_name

out_f = open("run-output.txt", 'w')

base_n = 100000

mpiruns = [
    MPIRun(100, 600, base_n, 1),
    MPIRun(100, 600, base_n, 2),
#     MPIRun(100, 600, base_n, 4),
#     MPIRun(100, 600, base_n, 8),
#     MPIRun(100, 600, base_n, 14),
#     MPIRun(100, 600, base_n, 16),
#     MPIRun(100, 600, base_n, 20)
]

for r in mpiruns:
    r.run()
    r.calc_speedup(mpiruns[0])
    r.output(out_f)

out_f.close()
