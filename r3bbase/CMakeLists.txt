##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

r3broot_add_library(R3Bbase
  SOURCES
  R3BModule.cxx
  R3BDetector.cxx
  R3BEventHeader.cxx
  R3BOnlineSpectra.cxx
  R3BOnlineSpectraDec2019.cxx
  R3BGlobalAnalysis.cxx
  R3BGlobalAnalysisS454.cxx
  # R3BOnlineSpectraLosTest.cxx

  LINKS
  PUBLIC
  FairRoot::GeoBase
  FairRoot::ParBase
  FairRoot::MbsAPI
  FairRoot::Base
  FairRoot::FairTools
  R3BRoot::R3BData
  ROOT::Core
  ROOT::Geom
  ROOT::GenVector
  ROOT::Physics
  ROOT::Matrix
  ROOT::MathCore
  R3BRoot::R3BTraRene

  LINKDEF R3BLinkDef.h)
