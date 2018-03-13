#!/bin/env python
from builtins import input
import ROOT

outfile = "./plot.pdf"

canvas = ROOT.TCanvas("glcanvas", "", int(1.6 * 400), 500)
ROOT.gStyle.SetOptStat(0)

expfile = ROOT.TFile.Open("exp/hist_29ne_27fn.20180313.root")
exphist = expfile.Get("hneutidq_exp").ProjectionY("pye", 1, 400)
exphist.SetTitle("NeuLAND Hit Energy p(^{29}Ne,^{27}F)2pn")
exphist.SetLabelFont(42)
exphist.GetXaxis().SetTitle("Energy [MeV]")
exphist.GetXaxis().SetTitleFont(42)
exphist.SetFillStyle(0)
exphist.SetLineColor(ROOT.kBlack)
expint = exphist.Integral(20, 30)
exphist.Draw("hist")

simfile = ROOT.TFile.Open("sim/output/digi.root")
simhist = simfile.Get("NeulandDigiMon/hE")
simint = simhist.Integral(20, 30)
simhist.Scale(expint / simint)
simhist.Draw("hist same")

legend = ROOT.TLegend(0.5, 0.5, 0.87, 0.6)
legend.AddEntry(simhist, "Simulation", "l")
legend.AddEntry(exphist, "Experiment", "l")
legend.SetLineWidth(0)
legend.Draw()

canvas.SaveAs(outfile)

input("Press enter to continue...")
