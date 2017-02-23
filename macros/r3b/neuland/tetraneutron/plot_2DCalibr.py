from __future__ import print_function
import math
import numpy
import sys
from collections import defaultdict
from builtins import input

import ROOT

dist = int(sys.argv[1])
dp = int(sys.argv[2])

file = "output/%dcm_%ddp_4n.digi.root" % (dist, dp)
parfile = "output/1400cm_%ddp.neutroncuts.par.root" % (dp)
outfile = "%dcm_%ddp_2DCalibr_4nCut.pdf" % (dist, dp)


tfile = ROOT.TFile.Open(file)
tparfile = ROOT.TFile.Open(parfile)

hist = tfile.Get("NeulandClusterMon/ClusterNumberEtot")

canvas = ROOT.TCanvas("glcanvas", "", 4 * 300, 3 * 300)
ROOT.gStyle.SetOptStat(0)

# reverse viridis (wrapping lists in numpy arrays works better with root)
# ROOT.gStyle.SetPalette(ROOT.kViridis)
stops = numpy.array([0.0000, 0.1250, 0.2500, 0.3750,
                     0.5000, 0.6250, 0.7500, 0.8750, 1.0000])
red = numpy.array([26. / 255., 51. / 255.,  43. / 255.,  33. / 255.,
                   28. / 255.,  35. / 255.,  74. / 255., 144. / 255., 246. / 255.][::-1])
green = numpy.array([9. / 255., 24. / 255.,  55. / 255.,  87. / 255.,
                     118. / 255., 150. / 255., 180. / 255., 200. / 255., 222. / 255.][::-1])
blue = numpy.array([30. / 255., 96. / 255., 112. / 255., 114. / 255.,
                    112. / 255., 101. / 255.,  72. / 255.,  35. / 255.,   0. / 255.][::-1])
ROOT.TColor.CreateGradientColorTable(9, stops, red, green, blue, 255, 1)


labels = ROOT.TLatex()
#labels.SetTextSize(0.04)

lines = ROOT.TLine()
lines.SetLineWidth(2)


pad1 = ROOT.TPad("pad1", "", 0.0, 0.0, 1.0, 1.0)
pad1.Draw()
pad1.cd()
hist.Draw("colz")
labels.DrawLatex(1800, 45, "%ddp" % dp)

for i in [1, 2, 3, 4]:
    cut = tparfile.Get("R3BNeulandNeutron2DPar").GetNeutronCuts()[i]
    w = cut.IntegralHist(hist)
    labels.DrawLatex(70, i * 11 - 7, "%2d%%" % (w / 100))
    cut.Draw("same")

canvas.SaveAs(outfile)

#input("Press enter to continue...")
