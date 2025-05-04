/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include <FairGenerator.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class FairPrimaryGenerator;

class R3BINCLRootGenerator : public FairGenerator
{
  public:
    /// Default constructor without arguments should not be used
    R3BINCLRootGenerator();

    /** Standard constructor.
     ** @param fileName The input file name
     **/
    explicit R3BINCLRootGenerator(const std::string& fileName);
    explicit R3BINCLRootGenerator(const TString& fileName);
    explicit R3BINCLRootGenerator(const char* fileName); // for old macros

    // Destructor
    ~R3BINCLRootGenerator() override;

    /** Reads on event from the input file and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param primGen  pointer to the R3BPrimaryGenerator
     **/
    bool ReadEvent(FairPrimaryGenerator* primGen) override;

    /**
     ** Set vertex point
     **/
    void SetXYZ(Double32_t x = 0, Double32_t y = 0, Double32_t z = 0);

    /**
     ** Set dispersion for the vertex point
     ** Gaussian distribution for x and y
     ** Uniform distribution for z, between z - sz and z + sz
     **/
    void SetDxDyDz(Double32_t sx = 0, Double32_t sy = 0, Double32_t sz = 0);

    /**
     ** Method to simulate only fission events
     **/
    void SetOnlyFission(bool Opt = true) { fOnlyFission = Opt; }

    /**
     **  Method to simulate only spallation events
     **/
    void SetOnlySpallation(bool Opt = true) { fOnlySpallation = Opt; }

    /**
     **  Method to simulate only p2p-fission events
     **/
    void SetOnlyP2pFission(bool Opt = true) { fOnlyP2pFission = Opt; }

  private:
    TString fFileName; // Input file name
    TFile* fInput;
    bool fOnlyFission = false;    // True if we want to simulate only fission events
    bool fOnlyP2pFission = false; // True if we want to simulate only p2p-fission events
    bool fOnlySpallation = false; // True if we want to simulate only spallation events

    /** Private method RegisterIons. Goes through the input file and registers
     ** any ion needed. TODO: Should not be needed by FairRoot. **/
    void RegisterIons();

    inline Int_t GetIonPdgId(int z, int a) { return 1000000000 + 10000 * z + 10 * a; }

    Int_t fEvt = 0;
    TTree* Tree;
    Int_t fEvtRoot = 0;
    Short_t fParticles;
    Short_t fOrigin[160];
    Short_t fMass[160];
    Short_t fCharge[160];
    Int_t fPdgCode[160];
    Float_t fPzPrime[160];
    Float_t fThetaPrime[160];
    Float_t fPhi[160];

    Double32_t fX = 0., fY = 0., fZ = 0.;    // Point vertex coordinates [cm]
    bool fPointVtxIsSet = false;             // True if point vertex is set
    Double32_t fDX = 0., fDY = 0., fDZ = 0.; // Point vertex coordinates [cm]
    bool fBoxVtxIsSet = false;               // True if point vertex is set

  public:
    ClassDefOverride(R3BINCLRootGenerator, 0); // NOLINT
};
