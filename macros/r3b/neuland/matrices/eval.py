from __future__ import print_function
import ROOT
import sys
import re

print("# dist", "ndp", "nn", "E", "mc_eff", "digi_eff", sep="\t")

for infile in sys.argv[1:]:
    m = re.search(
        '.*\/(\d*)cm_(\d*)dp_(\d)n_(\d*)AMeV_500keV\.digi\.root', infile)

    tfile = ROOT.TFile.Open(infile)

    try:
        print(m.group(1), m.group(2), m.group(3), m.group(4), sep="\t", end="\t")

        mc_hEtot = tfile.Get("NeulandMCMon/hE_tot")
        mc_efficiency = 1. - \
            (mc_hEtot.GetBinContent(1) / mc_hEtot.GetEntries())
        print(mc_efficiency, end="\t")

        digi_hEtot = tfile.Get("NeulandDigiMon/hEtot")
        digi_efficiency = 1. - \
            (digi_hEtot.GetBinContent(1) / digi_hEtot.GetEntries())
        print(digi_efficiency, end="\n")

    except:
        pass
