// -------------------------------------------------------------------------
// -----                      R3BMCTrack source file                   -----
// -----                  Created 03/08/04                             -----
// -------------------------------------------------------------------------
#include "R3BMCTrack.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
R3BMCTrack::R3BMCTrack()
    : fPdgCode(0)
    , fMotherId(-1)
    , fPx(0.)
    , fPy(0.)
    , fPz(0.)
    , fStartX(0.)
    , fStartY(0.)
    , fStartZ(0.)
    , fStartT(0.)
    , fNPoints(0)
    , fMass(0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMCTrack::R3BMCTrack(Int_t pdgCode,
                       Int_t motherId,
                       Double_t px,
                       Double_t py,
                       Double_t pz,
                       Double_t x,
                       Double_t y,
                       Double_t z,
                       Double_t t,
                       Int_t nPoints = 0)
    : fPdgCode(pdgCode)
    , fMotherId(motherId)
    , fPx(px)
    , fPy(py)
    , fPz(pz)
    , fStartX(x)
    , fStartY(y)
    , fStartZ(z)
    , fStartT(t)
    , fNPoints(0)
    , fMass(0.)
{
    if (nPoints >= 0)
    {
        fNPoints = nPoints;
    }
    else
    {
        fNPoints = 0;
    }
}
// -------------------------------------------------------------------------

// -----   Copy constructor   ----------------------------------------------
R3BMCTrack::R3BMCTrack(const R3BMCTrack& right)
    : fPdgCode(right.fPdgCode)
    , fMotherId(right.fMotherId)
    , fPx(right.fPx)
    , fPy(right.fPy)
    , fPz(right.fPz)
    , fStartX(right.fStartX)
    , fStartY(right.fStartY)
    , fStartZ(right.fStartZ)
    , fStartT(right.fStartT)
    , fNPoints(right.fNPoints)
    , fMass(right.fMass)
{
}
// -------------------------------------------------------------------------

// -----   Constructor from TParticle   ------------------------------------
R3BMCTrack::R3BMCTrack(TParticle* part, Int_t fMC)
    : fPdgCode(part->GetPdgCode())
    , fMotherId(part->GetMother(0))
    , fPx(part->Px())
    , fPy(part->Py())
    , fPz(part->Pz())
    , fStartX(part->Vx())
    , fStartY(part->Vy())
    , fStartZ(part->Vz())
    , fStartT(0.)
    , fNPoints(0)
    , fMass(part->GetMass())
{
    if (fMC == 0)
    {
        // G3
        fStartT = part->T() * 1e09;
    }
    else if (fMC == 1)
    {
        // G4
        fStartT = part->T();
    }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BMCTrack::~R3BMCTrack()
{
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BMCTrack::Print(Option_t* option) const
{
    cout << "Track " << option << ", mother : " << fMotherId << ", Type " << fPdgCode << ", momentum (" << fPx << ", "
         << fPy << ", " << fPz << ") GeV" << endl;
    cout << "       Ref " << GetNPoints(kREF) << ", DCH " << GetNPoints(kDCH) << ", CAL " << GetNPoints(kCAL)
         << ", LAND " << GetNPoints(kLAND) << ", GFI " << GetNPoints(kGFI) << ", mTOF " << GetNPoints(kMTOF)
         << ", dTOF " << GetNPoints(kDTOF) << ", TOF " << GetNPoints(kTOF) << ", TRACKER " << GetNPoints(kTRA)
         << ", CALIFA " << GetNPoints(kCALIFA) << ", MFI " << GetNPoints(kMFI) << ", PSP " << GetNPoints(kPSP)
         << ", VETO " << GetNPoints(kVETO) << ", STARTRACK " << GetNPoints(kSTARTRACK) << ", LUMON "
         << GetNPoints(kLUMON) << ", NeuLAND " << GetNPoints(kNEULAND) << endl;
#ifdef SOFIA
    cout << ", SCI " << GetNPoints(kSOFSCI) << ", AT " << GetNPoints(kSOFAT) << ", TRIM " << GetNPoints(kSOFTRIM) << ", MWPC1 " << GetNPoints(kSOFMWPC1) << ", TWIM " << GetNPoints(kSOFTWIM) << ", MWPC2 " << GetNPoints(kSOFMWPC2) << ", SOF ToF Wall " << GetNPoints(kSOFTofWall) << endl;
#endif
#ifdef GTPC
    cout << ", GTPC " << GetNPoints(kGTPC) << endl;
#endif
}
// -------------------------------------------------------------------------

// -----   Public method GetMass   -----------------------------------------
Double_t R3BMCTrack::GetMass() const
{
    //  if ( TDatabasePDG::Instance() ) {
    //    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
    //    if ( particle ) return particle->Mass();
    //    else return 0.;
    //  }
    //  return 0.;
    return fMass;
}
// -------------------------------------------------------------------------

// -----   Public method GetRapidity   -------------------------------------
Double_t R3BMCTrack::GetRapidity() const
{
    Double_t e = GetEnergy();
    Double_t y = 0.5 * TMath::Log((e + fPz) / (e - fPz));
    return y;
}
// -------------------------------------------------------------------------

// -----   Public method GetNPoints   --------------------------------------
Int_t R3BMCTrack::GetNPoints(DetectorId detId) const
{
    if (detId == kREF)
        return (fNPoints & 0x3);
    else if (detId == kDCH)
        return ((fNPoints & 0xC) >> 2);
    else if (detId == kCAL)
        return ((fNPoints & 0x30) >> 4);
    else if (detId == kLAND)
        return ((fNPoints & 0xC0) >> 6);
    else if (detId == kGFI)
        return ((fNPoints & 0x300) >> 8);
    else if (detId == kMTOF)
        return ((fNPoints & 0xC00) >> 10);
    else if (detId == kDTOF)
        return ((fNPoints & 0x3000) >> 12);
    else if (detId == kTOF)
        return ((fNPoints & 0xC000) >> 14);
    else if (detId == kTRA)
        return ((fNPoints & 0x30000) >> 16);
    else if (detId == kCALIFA)
        return ((fNPoints & 0xC0000) >> 18);
    else if (detId == kMFI)
        return ((fNPoints & 0x300000) >> 20);
    else if (detId == kPSP)
        return ((fNPoints & 0xC00000) >> 22);
    else if (detId == kVETO)
        return ((fNPoints & 0x3000000) >> 24);
    else if (detId == kSTARTRACK)
        return ((fNPoints & 0xC000000) >> 26);
    else if (detId == kLUMON)
        return ((fNPoints & 0x30000000) >> 28);
    else if (detId == kNEULAND)
        return ((fNPoints & 0xC0000000) >> 30);
    else if (detId == kACTAR)
        return ((fNPoints & 0x300000000) >> 32);
    else if (detId == kFI4)
        return ((fNPoints & 0xC00000000) >> 34);
    else if (detId == kFI6)
        return ((fNPoints & 0x3000000000) >> 36);
    else if (detId == kFI5)
        return ((fNPoints & 0xC000000000) >> 38);
    else if (detId == kSFI)
        return ((fNPoints & 0x30000000000) >> 40);
#ifdef SOFIA
    else if (detId == kSOFSCI)
        return ((fNPoints & 0xC0000000000) >> 42);
    else if (detId == kSOFAT)
        return ((fNPoints & 0x300000000000) >> 44);
    else if (detId == kSOFTRIM)
        return ((fNPoints & 0xC00000000000) >> 46);
    else if (detId == kSOFMWPC1)
        return ((fNPoints & 0x3000000000000) >> 48);
    else if (detId == kSOFTWIM)
        return ((fNPoints & 0xC000000000000) >> 50);
    else if (detId == kSOFMWPC2)
        return ((fNPoints & 0x30000000000000) >> 52);
    else if (detId == kSOFTofWall)
        return ((fNPoints & 0xC0000000000000) >> 54);
#endif
#ifdef GTPC
    else if (detId == kGTPC)
        return ((fNPoints & 0x300000000000000) >> 56);
#endif
    else
    {
        cout << "-E- R3BMCTrack::GetNPoints: Unknown detector ID " << detId << endl;
        return 0;
    }
}
// -------------------------------------------------------------------------

// -----   Public method SetNPoints   --------------------------------------
void R3BMCTrack::SetNPoints(Int_t iDet, Int_t nP)
{
    ULong_t nPoints = nP;
    if (nPoints > 3)
    {
        nPoints = 3;
    }

    if (iDet == kREF)
    {
        fNPoints = (fNPoints & (~0x3)) | nPoints;
    }
    else if (iDet == kDCH)
    {
        fNPoints = (fNPoints & (~0xC)) | (nPoints << 2);
    }
    else if (iDet == kCAL)
    {
        fNPoints = (fNPoints & (~0x30)) | (nPoints << 4);
    }
    else if (iDet == kLAND)
    {
        fNPoints = (fNPoints & (~0xC0)) | (nPoints << 6);
    }
    else if (iDet == kGFI)
    {
        fNPoints = (fNPoints & (~0x300)) | (nPoints << 8);
    }
    else if (iDet == kMTOF)
    {
        fNPoints = (fNPoints & (~0xC00)) | (nPoints << 10);
    }
    else if (iDet == kDTOF)
    {
        fNPoints = (fNPoints & (~0x3000)) | (nPoints << 12);
    }
    else if (iDet == kTOF)
    {
        fNPoints = (fNPoints & (~0xC000)) | (nPoints << 14);
    }
    else if (iDet == kTRA)
    {
        fNPoints = (fNPoints & (~0x30000)) | (nPoints << 16);
    }
    else if (iDet == kCALIFA)
    {
        fNPoints = (fNPoints & (~0xC0000)) | (nPoints << 18);
    }
    else if (iDet == kMFI)
    {
        fNPoints = (fNPoints & (~0x300000)) | (nPoints << 20);
    }
    else if (iDet == kPSP)
    {
        fNPoints = (fNPoints & (~0xC00000)) | (nPoints << 22);
    }
    else if (iDet == kVETO)
    {
        fNPoints = (fNPoints & (~0x3000000)) | (nPoints << 24);
    }
    else if (iDet == kSTARTRACK)
    {
        fNPoints = (fNPoints & (~0xC000000)) | (nPoints << 26);
    }
    else if (iDet == kLUMON)
    {
        fNPoints = (fNPoints & (~0x30000000)) | (nPoints << 28);
    }
    else if (iDet == kNEULAND)
    {
        fNPoints = (fNPoints & (~0xC0000000)) | (nPoints << 30);
    }
    else if (iDet == kACTAR)
    {
        fNPoints = (fNPoints & (~0x300000000)) | (nPoints << 32);
    }
    else if (iDet == kFI4)
    {
        fNPoints = (fNPoints & (~0xC00000000)) | (nPoints << 34);
    }
    else if (iDet == kFI6)
    {
        fNPoints = (fNPoints & (~0x3000000000)) | (nPoints << 36);
    }
    else if (iDet == kFI5)
    {
        fNPoints = (fNPoints & (~0xC000000000)) | (nPoints << 38);
    }
    else if (iDet == kSFI)
    {
        fNPoints = (fNPoints & (~0x30000000000)) | (nPoints << 40);
    }
#ifdef SOFIA
    else if (iDet == kSOFSCI)
    {
        fNPoints = (fNPoints & (~0xC0000000000)) | (nPoints << 42);
    }
    else if (iDet == kSOFAT)
    {
        fNPoints = (fNPoints & (~0x300000000000)) | (nPoints << 44);
    }
    else if (iDet == kSOFTRIM)
    {
        fNPoints = (fNPoints & (~0xC00000000000)) | (nPoints << 46);
    }
    else if (iDet == kSOFMWPC1)
    {
        fNPoints = (fNPoints & (~0x3000000000000)) | (nPoints << 48);
    }
    else if (iDet == kSOFTWIM)
    {
        fNPoints = (fNPoints & (~0xC000000000000)) | (nPoints << 50);
    }
    else if (iDet == kSOFMWPC2)
    {
        fNPoints = (fNPoints & (~0x30000000000000)) | (nPoints << 52);
    }
    else if (iDet == kSOFTofWall)
    {
        fNPoints = (fNPoints & (~0xC0000000000000)) | (nPoints << 54);
    }
#endif
#ifdef GTPC
    else if (iDet == kGTPC)
    {
        fNPoints = (fNPoints & (~0x300000000000000)) | (nPoints << 56);
    }
#endif
    else
    {
        cout << "-E- R3BMCTrack::SetNPoints: Unknown detector ID " << iDet << endl;
    }
}
// -------------------------------------------------------------------------

ClassImp(R3BMCTrack)
