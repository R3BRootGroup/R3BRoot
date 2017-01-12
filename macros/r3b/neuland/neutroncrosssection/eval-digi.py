from __future__ import print_function
import ROOT
import sys
import re

def main(argv):
    for infile in sys.argv[1:]:
        m = re.search('.*\/(.*?)-([0-9\.]*)MeV.mon.root', infile)

        tfile = ROOT.TFile.Open(infile)

        try:
            hist  = tfile.Get("NeulandDigiMon/hEtot")
            hist2 = tfile.Get("NeulandMCMon/hE_tot")
            print(m.group(1), m.group(2), hist.Integral(2, 10000) / hist.GetEntries(), hist2.Integral(2,10000) / hist2.GetEntries() )
        except:
            pass

if __name__ == "__main__":
    main(sys.argv)
