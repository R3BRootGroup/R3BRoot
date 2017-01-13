from __future__ import print_function
import ROOT
import sys
import re

def main(argv):
    for infile in sys.argv[1:]:
        m = re.search('.*\/(.*?)-([0-9\.]*)MeV.mon.root', infile)

        tfile = ROOT.TFile.Open(infile)
        #tree = tfile.Get("evt")

        # a = 0
        # b = 0
        # for event in tree:
        #     a += 1
        #     if event.NeulandDigis.GetEntries() > 0:
        #         b += 1
        try:
            hist = tfile.Get("NeulandDigiMon/hEtot")
            print(m.group(1), m.group(2), hist.Integral(2, 10000) / hist.GetEntries() )#, b/a)
        except:
            pass

if __name__ == "__main__":
    main(sys.argv)
