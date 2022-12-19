#!/usr/bin/python3
# read a log file, print from any lines containing "error:"
# to any line containing -Wfatal-errors.

import sys, re

start=re.compile("error:")
stop=re.compile("-Wfatal-errors")

fname=sys.argv[1]
print("Errors in "+fname)
p=False

f=open(fname)
for l in f:
    if start.search(l):
        p=True
    if p:
        print(l, end="")
        if stop.search(l):
            p=False
            print("----")
