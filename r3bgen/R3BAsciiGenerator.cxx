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

#include "R3BAsciiGenerator.h"
#include "FairIon.h"
#include "FairLogger.h"
#include "FairPrimaryGenerator.h"
#include "FairRunSim.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TRandom.h"

R3BAsciiGenerator::R3BAsciiGenerator()
    : fInputFile()
    , fFileName()
    , fPDG(nullptr)
    , fIonMap()
    , fX(0.)
    , fY(0.)
    , fZ(0.)
    , fPointVtxIsSet(false)
    , fDX(0.)
    , fDY(0.)
    , fDZ(0.)
    , fBoxVtxIsSet(false)
{
}

R3BAsciiGenerator::R3BAsciiGenerator(std::string fileName)
    : fInputFile()
    , fFileName(std::move(fileName))
    , fPDG(TDatabasePDG::Instance())
    , fIonMap()
    , fX(0.)
    , fY(0.)
    , fZ(0.)
    , fPointVtxIsSet(false)
    , fDX(0.)
    , fDY(0.)
    , fDZ(0.)
    , fBoxVtxIsSet(false)
{
    LOG(INFO) << "R3BAsciiGenerator: Looking for ions...";
    const int nIons = RegisterIons();
    LOG(INFO) << "R3BAsciiGenerator: " << nIons << " ions registered.";
}

R3BAsciiGenerator::R3BAsciiGenerator(const TString& fileName)
    : R3BAsciiGenerator(std::string(fileName))
{
}

R3BAsciiGenerator::R3BAsciiGenerator(const char* fileName)
    : R3BAsciiGenerator(std::string(fileName))
{
}

R3BAsciiGenerator::~R3BAsciiGenerator() { fInputFile.close(); }

bool R3BAsciiGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
    CheckAndOpen();

    // Define event variable to be read from file
    Int_t eventId = -1;
    Int_t nTracks = -1;
    Double_t pBeam = 0.;
    Double_t b = 0.;

    // Define track variables to be read from file
    Int_t iPid = -1;
    Int_t iA = 0;
    Int_t iZ = 0;
    Double_t px = 0.;
    Double_t py = 0.;
    Double_t pz = 0.;
    Double_t vx = 0.;
    Double_t vy = 0.;
    Double_t vz = 0.;
    Double_t iMass = 0.;

    // Read event header line from input file
    fInputFile >> eventId >> nTracks >> pBeam >> b;

    // Can happen at the end of the file (empty line)
    // Note: This is not really resilient to ill-formed files
    if (fInputFile.fail())
    {
        CheckAndOpen();
        fInputFile >> eventId >> nTracks >> pBeam >> b;
    }

    LOG(INFO) << "R3BAsciiGenerator: Reading Event: " << eventId << ",  pBeam = " << pBeam << "GeV, b = " << b
              << " fm, multiplicity " << nTracks;

    // Loop over tracks in the current event
    for (int itrack = 0; itrack < nTracks; itrack++)
    {
        if (fInputFile.eof())
        {
            // Failsafe
            continue;
        }
        fInputFile >> iPid >> iZ >> iA >> px >> py >> pz >> vx >> vy >> vz >> iMass;

        Int_t pdgType = 0;

        // Ion case ( iPid = -1 )
        if (iPid < 0)
        {
            TString ionName;
            if (1 == iZ && 2 == iA)
            {
                ionName = "Deuteron";
            }
            else if (1 == iZ && 3 == iA)
            {
                ionName = "Triton";
            }
            else if (2 == iZ && 3 == iA)
            {
                ionName = "HE3";
            }
            else if (2 == iZ && 4 == iA)
            {
                ionName = "Alpha";
            }
            else
            {
                ionName = TString::Format("Ion_%d_%d", iA, iZ);
            }
            TParticlePDG* part = fPDG->GetParticle(ionName);
            if (!part)
            {
                LOG(WARNING) << "R3BAsciiGenerator::ReadEvent: Cannot find " << ionName << " in database!";
                continue;
            }
            pdgType = part->PdgCode();
        }
        // else pdgType = iPid;  // "normal" particle
        else
        {
            pdgType = iA; // "normal" particle
        }

        if (fPointVtxIsSet)
        {
            vx = fX;
            vy = fY;
            vz = fZ;
            if (fBoxVtxIsSet)
            {
                vx = gRandom->Gaus(fX, fDX);
                vy = gRandom->Gaus(fY, fDY);
                vz = gRandom->Gaus(fZ, fDZ);
            }
        }

        primGen->AddTrack(pdgType, px, py, pz, vx, vy, vz);

    } //! tracks

    return true;
}

int R3BAsciiGenerator::RegisterIons()
{
    CheckAndOpen();

    Int_t nIons = 0;
    Int_t eventId, nTracks;
    Double_t pBeam, b;

    // Define track variables to be read from file
    Int_t iPid = -1;
    Int_t iA = 0;
    Int_t iZ = 0;
    Double_t px = 0.;
    Double_t py = 0.;
    Double_t pz = 0.;
    Double_t vx = 0.;
    Double_t vy = 0.;
    Double_t vz = 0.;
    Double_t iMass = 0.;

    fIonMap.clear();

    TString ionName;

    while (!fInputFile.eof())
    {
        fInputFile >> eventId >> nTracks >> pBeam >> b;

        for (Int_t iTrack = 0; iTrack < nTracks; iTrack++)
        {
            fInputFile >> iPid >> iZ >> iA >> px >> py >> pz >> vx >> vy >> vz >> iMass;

            // Ion Case
            if (iPid < 0)
            {
                ionName = TString::Format("Ion_%d_%d", iA, iZ);
                if (fIonMap.find(ionName) == fIonMap.end())
                {
                    auto ion = new FairIon(ionName, iZ, iA, iZ, 0., iMass);
                    fIonMap[ionName] = ion;
                    nIons++;
                } // new ion
            }     // ion
        }         // !tracks

    } //! events

    FairRunSim* run = FairRunSim::Instance();
    for (const auto& mapIt : fIonMap)
    {
        run->AddNewIon(mapIt.second);
    }

    fInputFile.close();

    return nIons;
}

void R3BAsciiGenerator::SetXYZ(Double32_t x, Double32_t y, Double32_t z)
{
    fX = x;
    fY = y;
    fZ = z;
    fPointVtxIsSet = kTRUE;
}

void R3BAsciiGenerator::SetDxDyDz(Double32_t sx, Double32_t sy, Double32_t sz)
{
    fDX = sx;
    fDY = sy;
    fDZ = sz;
    fBoxVtxIsSet = kTRUE;
}

void R3BAsciiGenerator::CheckAndOpen()
{
    // Open file or re-open if end reached
    if (fInputFile.eof() || !fInputFile.is_open())
    {
        LOG(INFO) << "R3BAsciiGenerator: Reading input file " << fFileName;
        fInputFile.close();
        fInputFile.open(fFileName);
    }

    // Check for input file
    if (!fInputFile.is_open())
    {
        LOG(FATAL) << "R3BAsciiGenerator: Input file not open!";
    }
}

ClassImp(R3BAsciiGenerator)
