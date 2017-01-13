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
outfile = "output/%dcm_%ddp_%dn_%dAMeV_%dkeV_cluster.pdf" % (distance, nDoublePlanes, nNeutrons, energy, erel)

tfile = ROOT.TFile.Open(file)


canvas = ROOT.TCanvas("glcanvas", "", 5 * 300, 2 * 300)
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




hist = tfile.Get("NeulandClusterMon/ClusterEToFVSTime")

pad1 = ROOT.TPad("pad1", "", 0.0, 0.0, 1.0, 1.0)
labels = ROOT.TLatex()
pad1.Divide(3, 1)
pad1.Draw()


pad1.cd(1)
pad1.SetLogz()
hist.GetYaxis().SetRangeUser(50,150)
hist.Draw("colz")


pad1.cd(2)
px = hist.ProjectionX()
px.Draw("colz")

pad1.cd(3)
py = hist.ProjectionY()
py.Draw("colz")


mlabels = ROOT.TLatex()
mlabels.SetTextSize(0.07)
mlabels.SetTextAlign(32)
mlabels.DrawLatexNDC(0.89, 0.86, "%2dm, %ddp" % (distance/100, nDoublePlanes))

canvas.SaveAs(outfile)

#input("Press enter to continue...")
