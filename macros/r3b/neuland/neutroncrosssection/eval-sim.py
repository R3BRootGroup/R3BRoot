from __future__ import print_function
import ROOT
import sys
import re

def main(argv):
    for infile in sys.argv[1:]:
        m = re.search('.*\/(.*?)-([0-9\.]*)MeV.sim.root', infile)

        tfile = ROOT.TFile.Open(infile)
        tree = tfile.Get("evt")

        try:
            a = 0
            b = 0
            c = 0
            for event in tree:
                a += 1
                if event.MCTrack.GetEntries() > 1:
                    b += 1
                if event.NeulandPoints.GetEntries() > 0:
                    c += 1

            print(m.group(1), m.group(2), b/a, c/a)
        except:
            pass

if __name__ == "__main__":
    main(sys.argv)
