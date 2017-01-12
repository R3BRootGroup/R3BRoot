#!/bin/env python
from __future__ import print_function

import sys
import neuland_pb2 as neuland
import ROOT
import stream

def read_and_append(tree, ostream):
  for rootevent in tree:
    protoevent = neuland.Event()
    protoevent.runID = rootevent.EventHeader.GetRunId()
    protoevent.eventID = rootevent.EventHeader.GetMCEntryNumber()
    for pixel in rootevent.NeulandPixels:
      p = protoevent.pixels.add()
      p.x = pixel.GetX()
      p.y = pixel.GetY()
      p.z = pixel.GetZ()
      p.e = pixel.GetE()
      p.t = pixel.GetT()
    for neutron in rootevent.NeulandPrimaryNeutronInteractionPixels:
      n = protoevent.neutrons.add()
      n.x = neutron.GetX()
      n.y = neutron.GetY()
      n.z = neutron.GetZ()
      n.t = neutron.GetT()
    for digi in rootevent.NeulandDigis:
      b = protoevent.digis.add()
      b.id = digi.GetPaddle()
      b.tl = digi.GetTdcL()
      b.tr = digi.GetTdcR()
      b.e = digi.GetE()
    ostream.write(protoevent)


print("Writing to protobuf stream %s" % sys.argv[1])
ostream = stream.open(sys.argv[1], 'wb')

for digifilename in sys.argv[2:]:
  simufilename = digifilename.replace('.digi.', '.simu.')

  print("Reading ROOT file %s" % digifilename)
  tfile = ROOT.TFile.Open(digifilename)
  ttree = tfile.Get("evt")
  print("... and ROOT file %s" % simufilename)
  ttree.AddFriend("simtree = evt", simufilename)

  read_and_append(ttree, ostream)

ostream.close()
