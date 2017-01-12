import sys
from builtins import input
import ROOT

path = str(sys.argv[1])
distance = int(sys.argv[2])
energy = int(sys.argv[3])
erel = int(sys.argv[4])
nDoublePlanes = int(sys.argv[5])
nNeutrons = int(sys.argv[6])

inrfile = "%s/%dcm_%ddp_%dn_%dAMeV_%dkeV.eval.root" % (path, distance, nDoublePlanes, nNeutrons, energy, erel)
outfile = "./%dcm_%ddp_%dn_%dAMeV_%dkeV.pdf" % (distance, nDoublePlanes, nNeutrons, energy, erel)

tfile = ROOT.TFile.Open(inrfile)
hist_sco = tfile.Get("%s/fhErel%d" % ("NeulandRecoMonScoring", nNeutrons))
hist_tdr = tfile.Get("%s/fhErel%d" % ("NeulandRecoMonTDR", nNeutrons))
hist_cht = tfile.Get("%s/fhErel%d" % ("NeulandRecoMonCheat", nNeutrons))

canvas = ROOT.TCanvas("glcanvas")
ROOT.gStyle.SetOptStat(0)

hist_cht.Rebin(10)
hist_cht.SetLineColor(ROOT.kGray)
hist_cht.SetTitle("")
hist_cht.GetXaxis().SetTitle("E_{rel} [keV]")
hist_cht.GetXaxis().SetRangeUser(0, 1100)
hist_cht.GetYaxis().SetTitle("Counts per 10 keV")
# hist_cht.GetYaxis().SetRangeUser(0, 400)
hist_cht.Draw()

hist_tdr.Rebin(10)
hist_tdr.SetLineColor(ROOT.kBlue)
hist_tdr.Draw("same")

fit_tdr = ROOT.TF1("fit_tdr", "gaus")
hist_tdr.Fit(fit_tdr, "W", "hist same", 0, 200)
fit_tdr.Draw("same")

labels_tdr = ROOT.TLatex()
labels_tdr.DrawLatexNDC(0.4, 0.35, "TDR: A = %3.0f, M = %3.0f, \sigma = %2.0f" % (
fit_tdr.GetParameter("Constant"), fit_tdr.GetParameter("Mean"), fit_tdr.GetParameter("Sigma")))

hist_sco.Rebin(10)
hist_sco.SetLineColor(ROOT.kRed)
hist_sco.Draw("same")

fit_sco = ROOT.TF1("fit_sco", "gaus")
hist_sco.Fit(fit_sco, "W", "hist same", 20, 250)
fit_sco.Draw("same")

labels_sco = ROOT.TLatex()
labels_sco.DrawLatexNDC(0.4, 0.4, "SCO: A = %3.0f, M = %3.0f, \sigma = %2.0f" % (
fit_sco.GetParameter("Constant"), fit_sco.GetParameter("Mean"), fit_sco.GetParameter("Sigma")))

legend = ROOT.TLegend(0.5, 0.5, 0.87, 0.7)
legend.AddEntry(hist_cht, "Best possible Reconstruction", "l")
legend.AddEntry(hist_tdr, "TDR like Reconstruction", "l")
legend.AddEntry(hist_sco, "Clusters Scoring 180602/0", "l")
legend.SetLineWidth(0)
legend.Draw()

labels = ROOT.TLatex()
labels.SetTextSize(0.05)
labels.SetTextAlign(32)
labels.DrawLatexNDC(0.89, 0.86, "%dSn + %dn, %d AMeV, %d keV" % (132 - nNeutrons, nNeutrons, energy, erel))
labels.DrawLatexNDC(0.89, 0.80, "%ddp @ %2dm" % (nDoublePlanes, distance / 100))

canvas.SaveAs(outfile)

input("Press enter to continue...")
