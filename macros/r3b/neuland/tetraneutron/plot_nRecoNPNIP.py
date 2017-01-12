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
directory = str(sys.argv[6])

file = "output/%dcm_%ddp_%dn_%dAMeV_%dkeV.eval.root" % (distance, nDoublePlanes, nNeutrons, energy, erel)
outfile = "./%dcm_%ddp_%dn_%dAMeV_%dkeV_nRecoNPNIPs_%s.pdf" % (distance, nDoublePlanes, nNeutrons, energy, erel, directory)

tfile = ROOT.TFile.Open(file)
hist = tfile.Get("%s/fhErelVSnNrecoNPNIPs" % directory)


canvas = ROOT.TCanvas("glcanvas", "", 4 * 300, 3 * 300)
ROOT.gStyle.SetOptStat(0)
#ROOT.gStyle.SetOptFit(1011);

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


pad1 = ROOT.TPad("pad1", "", 0.0, 0.0, 1.0, 1.0)
labels = ROOT.TLatex()
pad1.Divide(2, 2)
pad1.Draw()


pad1.cd(1)
hist.SetTitle("")
hist.GetXaxis().SetRangeUser(0, 2)
hist.GetXaxis().SetTitle("E_{rel} [MeV]")
hist.GetYaxis().SetRangeUser(0, 6)
hist.GetYaxis().SetTitle("Reconstructed Neutrons")
hist.Draw("colz")


pad1.cd(3)
py = hist.ProjectionY()
py.SetTitle("Number of reconstructed neutrons")
py.GetXaxis().SetRangeUser(0, 6)
py.GetXaxis().SetTitle("Reconstructed neutrons")
py.Draw()


pad1.cd(2)
px = hist.ProjectionX()
px.SetTitle("E_{rel}")
px.GetXaxis().SetRangeUser(0, 1)
px.GetXaxis().SetTitle("E_{rel} [MeV]")
px.Draw()
fit = px.Fit("gaus", "S", "", 0.04, 0.16)
try:
    labels.DrawLatexNDC(0.3, 0.5, "#sigma = %2d keV" %
                    (fit.GetParams()[2] * 1000))
except:
    pass

mlabels = ROOT.TLatex()
mlabels.SetTextSize(0.07)
mlabels.SetTextAlign(32)
mlabels.DrawLatexNDC(0.89, 0.86, "%2dm, %ddp" % (distance/100, nDoublePlanes))


pad1.cd(4)
pxc = hist.ProjectionX("_pxc", nNeutrons + 1, nNeutrons + 1)
pxc.SetTitle("E_{rel} with gate on %d Neutrons" % nNeutrons)
pxc.GetXaxis().SetRangeUser(0, 1)
pxc.GetXaxis().SetTitle("E_{rel} [MeV]")
pxc.Draw()
fitc = pxc.Fit("gausn", "S", "", 0.9*erel/1000., 1.1*erel/1000.)
#labels.DrawLatexNDC(0.5, 0.7, "Integral %d" % int(pxc.Integral(1,pxc.FindBin(0.2))) )
#labels.DrawLatexNDC(0.3, 0.7, "A = %d" % int(fitc.GetParams()[0]) )
#labels.DrawLatexNDC(0.3, 0.6, "E = %d keV" % (fitc.GetParams()[1] * 1000))
try:
	labels.DrawLatexNDC(0.3, 0.5, "#sigma = %2d keV" % (fitc.GetParams()[2] * 1000))
except:
    pass

canvas.SaveAs(outfile)

input("Press enter to continue...")
