/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// Configuration macro for setting common cuts and processes for G3, G4 and Fluka.
// Specific cuts and processes to G3 or G4 should be set in the g3Config.C, g4Config.C or flConfig.C

void SetCuts()
{
 // Switch on/off the physics processes (global)
  // Please consult the file data/galice.cuts for detector
  // specific settings, i.e. DRAY
  gMC->SetProcess("DCAY",1); // Particle decay
  gMC->SetProcess("PAIR",1); // Pair production
  gMC->SetProcess("COMP",1); // Compton scattering
  gMC->SetProcess("PHOT",1); // Photo effect
  gMC->SetProcess("PFIS",0); // Photo fission
  gMC->SetProcess("DRAY",1); // Delta rays
  gMC->SetProcess("ANNI",1); // Positron annihilation
  gMC->SetProcess("BREM",1); // Bremstrahlung
  gMC->SetProcess("MUNU",1); // Muon nuclear interactions
  gMC->SetProcess("CKOV",1); // Cerenkov production
  gMC->SetProcess("HADR",5); // Hadronic interactions
  gMC->SetProcess("LOSS",2); // Energy loss (2=complete fluct.)
  gMC->SetProcess("MULS",1); // Multiple scattering
  gMC->SetProcess("RAYL",1); // Rayleigh scattering

    cout << "[SetCuts]: Setting Cuts ..." << endl;

    const Double_t cut1 = 1.0E-4;  // GeV --> 100 keV
    const Double_t cut2 = 1.0E-3;  // GeV --> 1 MeV
    const Double_t tofmax = 1.E10; // seconds

    gMC->SetCut("CUTGAM", cut1);   /** gammas (GeV)*/
    gMC->SetCut("CUTELE", cut2);   /** electrons (GeV)*/
    gMC->SetCut("CUTNEU", cut1);   /** neutral hadrons (GeV)*/
    gMC->SetCut("CUTHAD", cut2);   /** charged hadrons (GeV)*/
    gMC->SetCut("CUTMUO", cut2);   /** muons (GeV)*/
    gMC->SetCut("BCUTE", cut2);    /** electron bremsstrahlung (GeV)*/
    gMC->SetCut("BCUTM", cut2);    /** muon and hadron bremsstrahlung(GeV)*/
    gMC->SetCut("DCUTE", cut2);    /** delta-rays by electrons (GeV)*/
    gMC->SetCut("DCUTM", cut2);    /** delta-rays by muons (GeV)*/
    gMC->SetCut("PPCUTM", cut2);   /** direct pair production by muons (GeV)*/
    gMC->SetCut("TOFMAX", tofmax); /**time of flight cut in seconds*/
}
