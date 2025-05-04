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

// -------------------------------------------------------------------------
// -----              R3BCryAsciiGenerator header file                 -----
// -----  This is a cosmic ray generator taking as input a file from   -----
// -----  the Cosmic-ray Shower Library (CRY):                         -----
// -----  https://nuclear.llnl.gov/simulation/                         -----
// -------------------------------------------------------------------------

#pragma once

#include <FairGenerator.h>
#include <TString.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

class FairPrimaryGenerator;

class R3BCryAsciiGenerator : public FairGenerator
{
  public:
    /** Default constructor without arguments should not be used. **/
    R3BCryAsciiGenerator();

    /** Standard constructor.
     ** @param fileName The input file name
     **/
    explicit R3BCryAsciiGenerator(const std::string& fileName);
    explicit R3BCryAsciiGenerator(const TString& fileName);
    explicit R3BCryAsciiGenerator(const char* fileName);

    /** Destructor. **/
    ~R3BCryAsciiGenerator() override;

    /** Reads on event from the input file and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param primGen  pointer to the R3BPrimaryGenerator
     **/
    bool ReadEvent(FairPrimaryGenerator* primGen) override;

    // Set origin for cosmic rays
    void SetTopOrigin(float dist) { fTopDist = dist; };

  private:
    TString fFileName;
    ifstream infile;
    float fTopDist = 0.; // Origin of cosmic rays in the Y-coordinate

    /** Private method CloseInput. Closes the input file properly.
     ** Called from destructor and from ReadEvent. **/
    void CloseInput();

  public:
    ClassDefOverride(R3BCryAsciiGenerator, 0);
};
