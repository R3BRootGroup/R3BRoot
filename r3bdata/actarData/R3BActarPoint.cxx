// -------------------------------------------------------------------------
// -----                     R3BActarPoint source file                 -----
// -------------------------------------------------------------------------

#include "R3BActarPoint.h"

#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
R3BActarPoint::R3BActarPoint()
    : FairMCPoint()
    , fModule(-1)
    , fX_out(0.)
    , fY_out(0.)
    , fZ_out(0.)
    , fPx_out(0.)
    , fPy_out(0.)
    , fPz_out(0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BActarPoint::R3BActarPoint(Int_t trackID,
                             Int_t detID,
                             Int_t plane,
                             TVector3 posIn,
                             TVector3 posOut,
                             TVector3 momIn,
                             TVector3 momOut,
                             Double_t tof,
                             Double_t length,
                             Double_t eLoss)
    : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss)
    , fModule(plane)
    , fX_out(posOut.X())
    , fY_out(posOut.Y())
    , fZ_out(posOut.Z())
    , fPx_out(momOut.Px())
    , fPy_out(momOut.Py())
    , fPz_out(momOut.Pz())
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BActarPoint::~R3BActarPoint()
{
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BActarPoint::Print(const Option_t* opt) const
{
    LOG(INFO) << "R3BActarPoint: STS Point for track " << fTrackID << " in detector " << fDetectorID << FairLogger::endl
              << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << FairLogger::endl
              << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << FairLogger::endl
              << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV"
              << FairLogger::endl;
}
// -------------------------------------------------------------------------

// -----   Point x coordinate from linear extrapolation   ------------------
Double_t R3BActarPoint::GetX(Double_t z) const
{
    //  cout << fZ << " " << z << " " << fZ_out << endl;
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fX_out + fX) / 2.;
    Double_t dz = fZ_out - fZ;
    return (fX + (z - fZ) / dz * (fX_out - fX));
}
// -------------------------------------------------------------------------

// -----   Point y coordinate from linear extrapolation   ------------------
Double_t R3BActarPoint::GetY(Double_t z) const
{
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fY_out + fY) / 2.;
    Double_t dz = fZ_out - fZ;
    //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
    return (fY + (z - fZ) / dz * (fY_out - fY));
}
// -------------------------------------------------------------------------

// -----   Public method IsUsable   ----------------------------------------
Bool_t R3BActarPoint::IsUsable() const
{
    Double_t dz = fZ_out - fZ;
    if (TMath::Abs(dz) < 1.e-4)
        return kFALSE;
    return kTRUE;
}
// -------------------------------------------------------------------------

ClassImp(R3BActarPoint)
