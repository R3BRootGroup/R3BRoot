/******************************************************************************
 *   Copyright (C) 2021 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2021-2026 Members of R3B Collaboration                     *
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
    void SetXYZ(double x = 0, double y = 0, double z = 0);

    /**
     ** Set dispersion for the vertex point
     ** Gaussian distribution for x and y
     ** Uniform distribution for z, between z - sz and z + sz
     **/
    void SetDxDyDz(double sx = 0, double sy = 0, double sz = 0);

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

    /**
     **  Method to simulate only fragments (changed with SetMinPdgCode)
     **/
    void SetOnlyfragments(bool Opt = true) { fOnlyFragments = Opt; }
    void SetMinPdgCode(int Opt) { fPdgCodeMin = Opt; }

  private:
    TString fFileName; // Input file name
    TFile* fInput;
    bool fOnlyFission = false;    // True if we want to simulate only fission events
    bool fOnlyP2pFission = false; // True if we want to simulate only p2p-fission events
    bool fOnlySpallation = false; // True if we want to simulate only spallation events
    bool fOnlyFragments = false;  // True if we want to simulate only fragments
    int fPdgCodeMin = 1000050070; // Limit in Boro-7

    /** Private method RegisterIons. Goes through the input file and registers
     ** any ion needed. TODO: Should not be needed by FairRoot. **/
    void RegisterIons();

    inline Int_t GetIonPdgId(int z, int a) { return 1000000000 + 10000 * z + 10 * a; }

    int64_t fEvt = 0;
    TTree* Tree;
    int64_t fEvtRoot = 0;
    Short_t fParticles;
    Short_t fOrigin[160];
    Short_t fMass[160];
    Short_t fCharge[160];
    int fPdgCode[160];
    Float_t fPzPrime[160];
    Float_t fThetaPrime[160];
    Float_t fPhi[160];

    double fX = 0., fY = 0., fZ = 0.;    // Point vertex coordinates [cm]
    bool fPointVtxIsSet = false;         // True if point vertex is set
    double fDX = 0., fDY = 0., fDZ = 0.; // Point vertex coordinates [cm]
    bool fBoxVtxIsSet = false;           // True if point vertex is set

  public:
    ClassDefOverride(R3BINCLRootGenerator, 0); // NOLINT
};
