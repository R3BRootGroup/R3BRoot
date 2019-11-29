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

#ifndef R3BASCIIGENERATOR_H
#define R3BASCIIGENERATOR_H 1

#include "FairGenerator.h"
#include "TString.h"
#include <fstream>
#include <map>
#include <string>

class TDatabasePDG;
class FairPrimaryGenerator;
class FairIon;

class R3BAsciiGenerator : public FairGenerator
{
  public:
    /** Default constructor without arguments should not be used. **/
    R3BAsciiGenerator();

    /** Standard constructor.
     ** @param fileName The input file name
     **/
    explicit R3BAsciiGenerator(std::string fileName);
    explicit R3BAsciiGenerator(const TString& fileName);
    explicit R3BAsciiGenerator(const char* fileName); // for old macros

    /** Destructor. **/
    ~R3BAsciiGenerator() override;

    /** Reads on event from the input file and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param primGen  pointer to the R3BPrimaryGenerator
     **/
    bool ReadEvent(FairPrimaryGenerator* primGen) override;

    void SetXYZ(Double32_t x = 0, Double32_t y = 0, Double32_t z = 0);

    void SetDxDyDz(Double32_t sx = 0, Double32_t sy = 0, Double32_t sz = 0);

  private:
    std::ifstream fInputFile;    //! Input file stream
    const std::string fFileName; //! Input file Name
    TDatabasePDG* fPDG;          //!  PDG database (non-owning)

    /** Private method RegisterIons. Goes through the input file and registers
     ** any ion needed. **/
    int RegisterIons();

    void CheckAndOpen();

    /** STL map from ion name to FairIon **/
    std::map<TString, FairIon*> fIonMap; //!

    Double32_t fX, fY, fZ;    // Point vertex coordinates [cm]
    bool fPointVtxIsSet;      // True if point vertex is set
    Double32_t fDX, fDY, fDZ; // Point vertex coordinates [cm]
    bool fBoxVtxIsSet;        // True if point vertex is set

    ClassDefOverride(R3BAsciiGenerator, 1);
};

#endif
