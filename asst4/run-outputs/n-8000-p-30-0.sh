#!/bin/sh
#PBS -N n-8000-p-30-0
#PBS -j oe
#PBS -l walltime=2:00:00
#PBS -l select=1:ncpus=30

/home/juch/juch-s16/csci551/asst4/bin/gauss-eliminate 8000 30
