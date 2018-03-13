#!/bin/env python
from __future__ import print_function
import ROOT
import sys
import re


def main(argv):
    for infile in sys.argv[1:]:
        tfile = ROOT.TFile.Open(infile)
        m = re.search('.*\/(.*?)-([0-9\.]*)MeV-thresh([0-9\.]*).digi.root', infile)

        hist = tfile.Get("NeulandDigiMon/hEtot")
        print(m.group(1), m.group(2), m.group(3), hist.Integral(int(1 + float(m.group(3))), 10000) / hist.GetEntries())


if __name__ == "__main__":
    main(sys.argv)
