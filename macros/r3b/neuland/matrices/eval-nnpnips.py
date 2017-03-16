from __future__ import print_function
import ROOT
import sys
import re

print("# dist", "ndp", "nn", "e", "ni", "eff", sep="\t")

for infile in sys.argv[1:]:
    m = re.search(
        '.*\/(\d*)cm_(\d*)dp_(\d)n_(\d*)AMeV_500keV\.digi\.root', infile)

    tfile = ROOT.TFile.Open(infile)

    try:
        fhnNPNIPs = tfile.Get("NeulandMCMon/fhnNPNIPs")
        for i in range(0, int(m.group(3)) + 1):
            print(m.group(1), m.group(2), m.group(3), m.group(4),
                  i, fhnNPNIPs.GetBinContent(i + 1) / fhnNPNIPs.GetEntries(), sep="\t")
    except:
        pass
