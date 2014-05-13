#! /bin/env python
import os, sys

n     = 8000
pvals = [1, 2, 5, 10, 20, 30]

def get_executable():
    if len(sys.argv) < 2:
        print "usage: %s gauss-executable" % sys.argv[0]
        sys.exit()

    return sys.argv[1]

def gen_batch_file(n, p, i, exec_file):
    name = "n-%d-p-%d-%i.sh" % (n, p, i)
    f = open(name, "w")

    file_data = """#!/bin/sh
#PBS -N n-%(n)d-p-%(p)d-%(i)d
#PBS -j oe
#PBS -l walltime=2:00:00
#PBS -l select=1:ncpus=%(p)d

%(exec_name)s %(n)d %(p)d
""" % {
    "n"         : n,
    "p"         : p,
    "i"         : i,
    "exec_name" :exec_file
}

    f.write(file_data)
    f.close()

    return name

def main():

    exec_file = get_executable()

    for p in pvals:
        for i in xrange(5):
            batch_file =  gen_batch_file(n, p, i, exec_file)
            cmd  = "qsub " + batch_file
            print cmd
            os.system(cmd)

main()
