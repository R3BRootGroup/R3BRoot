#!/bin/env python
from __future__ import print_function
import ROOT
import sys
import numpy


stops = numpy.array([0.0000, 0.1250, 0.2500, 0.3750,
                     0.5000, 0.6250, 0.7500, 0.8750, 1.0000])
red = numpy.array([26. / 255., 51. / 255.,  43. / 255.,  33. / 255.,
                   28. / 255.,  35. / 255.,  74. / 255., 144. / 255., 246. / 255.][::-1])
green = numpy.array([9. / 255., 24. / 255.,  55. / 255.,  87. / 255.,
                     118. / 255., 150. / 255., 180. / 255., 200. / 255., 222. / 255.][::-1])
blue = numpy.array([30. / 255., 96. / 255., 112. / 255., 114. / 255.,
                    112. / 255., 101. / 255.,  72. / 255.,  35. / 255.,   0. / 255.][::-1])
ROOT.TColor.CreateGradientColorTable(9, stops, red, green, blue, 255, 1)
ROOT.gStyle.SetOptStat(0)


canvas = ROOT.TCanvas("glcanvas", "", 4 * 300, 3 * 300)
canvas.SetLogz()

tfile = ROOT.TFile.Open(sys.argv[1])
hist = tfile.Get("NeulandDigiMon/hdeltaEE")
hist.SetDirectory(0)

for infile in sys.argv[2:]:
    tfile = ROOT.TFile.Open(infile)
    hist.Add(tfile.Get("NeulandDigiMon/hdeltaEE"))


hist.SetTitle("#DeltaEE")
hist.GetXaxis().SetTitle("E_{tot} [MeV]")
hist.GetYaxis().SetTitle("#DeltaE (Energy in foremost paddle) [MeV]")

hist.Draw("COLZ")
canvas.SaveAs("deltaEE.pdf")

input("Press enter to continue...")
