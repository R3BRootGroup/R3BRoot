#!/usr/bin/python3
# read a log file, print from any lines containing "error:"
# to any line containing -Wfatal-errors.

import sys, re

start=re.compile("error:")
stop=re.compile("-Wfatal-errors")

fname=sys.argv[1]
print("[print_errors.py] Errors in "+fname)
p=False
found=False
f=open(fname)
backlog=[]
for l in f:
    if start.search(l):
        print('**********')
        for i in range(max(0, len(backlog)-10), len(backlog)):
            print(backlog[i], end='')
        p=True
        found=True
    if p:
        print(l, end="")
        if stop.search(l):
            p=False
            print("----")
            backlog=[]
    else:
        backlog.append(l)

if not found:
    print("[print_errors.py] I could not find any errors in %s, so here is the whole file:"%fname)
    f=open(fname)
    for l in f:
        print(l, end="")
    print("\n[print_errors.py] Hope that helps!\n")
