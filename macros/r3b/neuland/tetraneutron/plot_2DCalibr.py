from __future__ import print_function
import math
import numpy
import sys
from collections import defaultdict
from builtins import input

import ROOT

distance = int(sys.argv[1])
nDoublePlanes = int(sys.argv[2])
nNeutrons = int(sys.argv[3])
energy = int(sys.argv[4])
erel = int(sys.argv[5])

file    = "output/%dcm_%ddp_%dn_%dAMeV_%dkeV.digi.root"    % (distance, nDoublePlanes, nNeutrons, energy, erel)
parfile = "output/%dcm_%ddp_600AMeV_500keV.neutroncuts.para.root" % (distance, nDoublePlanes)
outfile = "./%dcm_%ddp_%dn_%dAMeV_%dkeV_2DCalibr.pdf" % (distance, nDoublePlanes, nNeutrons, energy, erel)

tfile = ROOT.TFile.Open(file)
tparfile = ROOT.TFile.Open(parfile)

print(tparfile.Get("R3BNeulandNeutron2DPar").GetNeutronCut(3))

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
labels.DrawLatex(1800, 45, "%ddp" % nDoublePlanes)

for i in [1, 2, 3, 4]:
    cut = tparfile.Get("R3BNeulandNeutron2DPar").GetNeutronCut(i)
    w = cut.IntegralHist(hist)
    labels.DrawLatex(70, i * 11 - 7, "%2d%%" % (w / 100))
    cut.Draw("same")

canvas.SaveAs(outfile)

input("Press enter to continue...")
