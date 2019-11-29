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

// -------------------------------------------------------------------------
// -----                      R3BFieldMap source file                  -----
// -----               Adapted from R3BSim <D.Bertini@gsi.de>
// -------------------------------------------------------------------------

// Includes from C
#include <fstream>
#include <iomanip>
#include <iostream>

// Includes from ROOT
#include "TArrayF.h"
#include "TFile.h"
#include "TMath.h"

// Includes from CBMROOT
#include "R3BFieldMap.h"
//#include "R3BFieldMapCreator.h"
#include "R3BFieldMapData.h"
#include "R3BFieldPar.h"
#include "TArrayI.h"

using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::ofstream;
using std::right;
using std::setw;
using std::showpoint;
using TMath::Nint;

// -------------   Default constructor  ----------------------------------
R3BFieldMap::R3BFieldMap()
{
    // do nothing ..
}
// ------------------------------------------------------------------------

// -------------   Standard constructor   ---------------------------------
R3BFieldMap::R3BFieldMap(const char* mapName, const char* fileType)
    : FairField(mapName)
{
    // empty ctor
}

// ------------------------------------------------------------------------
R3BFieldMap::R3BFieldMap(Int_t type, Bool_t verbosity)
    : FairField("R3Bmap")
{

    // specific to R3B to be consistent  with the geometry of the Aladin Magnet
    // Problem : Hardcoded parameters to avoid using the parameter container
    // factory <D.Bertini@gsi.de>

    fVerbose = verbosity;

    Double_t DistanceToTarget = 350.0; // cm
    // Double_t Correction = -119.94; // cm
    Double_t Correction = -95.0;  // cm
                                  // Double_t Aladin_gap = 50.;    //cm
                                  // Double_t Aladin_angle = +7.3; // degree
    Double_t Aladin_angle = -7.0; // degree
                                  // Double_t Yoke_thickness = 50.; //cm

    Double_t DistanceFromtargetToAladinCenter = DistanceToTarget + Correction;
    // Transformations inverse
    gRot = new TRotation();
    gRot->RotateY(-1. * Aladin_angle);
    gTrans = new TVector3(0.0, 0.0, -1. * DistanceFromtargetToAladinCenter);

    // Magnetic field map types definition
    typeField = type;

    cout << " -I- R3BFieldMap::ctor  " << endl;
    cout << " -I- R3BFieldMap::ctor  typeField:  " << typeField << endl;
    cout << " -I- R3BFieldMap::ctor  Aladin angle:  " << Aladin_angle << endl;

    // initial values
    initialX = 0.;
    initialY = 0.;
    initialZ = 0.;
    gridStep = 0.;
    stepsInX = 0;
    stepsInY = 0;
    stepsInZ = 0;

    Int_t numberOfPointsInGrid = 0;

    // -- ALADIN MAGNET --
    if (typeField == 0 || typeField == 3)
    {
        initialX = -65.0;
        initialY = -25.0;
        initialZ = -125.0;
        gridStep = 5.0;
        stepsInX = 27;
        stepsInY = 11;
        stepsInZ = 51;
        numberOfPointsInGrid = stepsInX * stepsInY * stepsInZ;
        Bxfield = new Double_t[numberOfPointsInGrid];
        Byfield = new Double_t[numberOfPointsInGrid];
        Bzfield = new Double_t[numberOfPointsInGrid];

        // reading the full field map

#include "magField/Aladin/ALADIN.dat"

        cout << " -I- R3BFieldMap::ctor  field map loaded ..  " << endl;
        cout << " -I- R3BFieldMap::ctor  filename: magField/Aladin/ALADIN.dat " << endl;

        if (typeField == 0)
        {
            // Normal
            for (Int_t i = 0; i < numberOfPointsInGrid; i++)
            {
                Bxfield[i] = xfield[i] * 1E-01 * 10.; // kG
                Byfield[i] = yfield[i] * 1E-01 * 10.; // kG
                Bzfield[i] = zfield[i] * 1E-01 * 10.; // kG
            };
        }
        else
        {
            // inverse values?
            for (Int_t i = 0; i < numberOfPointsInGrid; i++)
            {
                Bxfield[i] = -xfield[i] * 1E-01 * 10.; // kG
                Byfield[i] = -yfield[i] * 1E-01 * 10.; // kG
                Bzfield[i] = -zfield[i] * 1E-01 * 10.; // kG
            };
        }

        // verbose mode

        if (fVerbose == kTRUE)
        {
            for (Int_t i = 0; i < numberOfPointsInGrid; i++)
            {
                if (Bxfield[i] != 0 && Byfield[i] != 0 && Bzfield[i] != 0)
                {
                    cout << " Grid Point: " << i << " FieldX: " << Bxfield[i] << " FieldY: " << Byfield[i]
                         << " FieldZ: " << Bzfield[i] << endl;
                }
            }
        }
    }

    // -- R3B MAGNET --
    else if (typeField == 1)
    {
        initialX = -20.0;
        initialY = -20.0;
        initialZ = -100.0;
        gridStep = 3.0;
        stepsInX = 74;
        stepsInY = 34;
        stepsInZ = 167;
        numberOfPointsInGrid = stepsInX * stepsInY * stepsInZ;
        Bxfield = new Double_t[numberOfPointsInGrid];
        Byfield = new Double_t[numberOfPointsInGrid];
        Bzfield = new Double_t[numberOfPointsInGrid];

        // reading the full field map
        //#include "magField/R3B/R3B.dat"
        //
    }

    else if (typeField == 3)
    {
        // ALADIN MAGNET inverted (for backTracking)
        initialX = -65.0;
        initialY = -25.0;
        initialZ = -125.0;
        gridStep = 5.0;
        stepsInX = 27;
        stepsInY = 11;
        stepsInZ = 51;
        numberOfPointsInGrid = stepsInX * stepsInY * stepsInZ;
        Bxfield = new Double_t[numberOfPointsInGrid];
        Byfield = new Double_t[numberOfPointsInGrid];
        Bzfield = new Double_t[numberOfPointsInGrid];
        // reading the full field map
        //#include "magField/Aladin/ALADIN_INVERSE.dat"
        //
    }
}

// ------------------------------------------------------------------------
//
//    ReWriting of FieldMap class <D.Bertini@gsi.de>
// ------------------------------------------------------------------------

void R3BFieldMap::GetFieldValue(const Double_t point[3], Double_t* bField)
{
    // Main function to get the field values
    // not optimized  for the moment
    // <D.Bertini@gsi.de>

    Double_t Bfield[3] = { 0.0, 0.0, 0.0 };

    if (typeField == 0 || typeField == 1 || typeField == 3)
    {

        // local
        TArrayI* linesArray = new TArrayI(8);

        // local to global
        TVector3 localPoint(point[0], point[1], point[2]);

        // cout << "Local Point: " << localPoint.X() << " : " << localPoint.Y()
        //      << " : " << localPoint.Z()
        //      << endl;

        // localPoint.Transform(*gRot);
        localPoint = localPoint + (*gTrans); // First translation
        localPoint.Transform(*gRot);

        // test area
        if (localPoint.X() >= initialX && localPoint.Y() >= initialY && localPoint.Z() >= initialZ &&
            localPoint.X() <= initialX + ((stepsInX - 1) * gridStep) &&
            localPoint.Y() <= initialY + ((stepsInY - 1) * gridStep) &&
            localPoint.Z() <= initialZ + ((stepsInZ - 1) * gridStep))
        {

            //   cout << "-I-  inside  the magnetic area! " << endl;
            //   cout << "Transf. Point: " << localPoint.X() << " : " <<
            //        localPoint.Y() << " : " <<  localPoint.Z()
            //       << endl;

            Int_t returnValue = GetLinesArrayForPosition(&localPoint, linesArray);

            if (!returnValue)
            {
                TVector3 vertexReferenceInGrid;
                Int_t linpos = linesArray->At(0);
                if (GetPositionForLine(linpos, &vertexReferenceInGrid))
                {
                    cout << "-E-R3BFieldMap Line out of bound " << endl;
                }
                else
                {
                    Double_t t = (localPoint.X() - vertexReferenceInGrid.X()) / (gridStep);
                    Double_t u = (localPoint.Y() - vertexReferenceInGrid.Y()) / (gridStep);
                    Double_t v = (localPoint.Z() - vertexReferenceInGrid.Z()) / (gridStep);

                    Bfield[0] = (1 - t) * (1 - u) * (1 - v) * Bxfield[linesArray->At(0)] +
                                t * (1 - u) * (1 - v) * Bxfield[linesArray->At(1)] +
                                t * u * (1 - v) * Bxfield[linesArray->At(2)] + t * u * v * Bxfield[linesArray->At(3)] +
                                (1 - t) * u * (1 - v) * Bxfield[linesArray->At(4)] +
                                (1 - t) * u * v * Bxfield[linesArray->At(5)] +
                                (1 - t) * (1 - u) * v * Bxfield[linesArray->At(6)] +
                                t * (1 - u) * v * Bxfield[linesArray->At(7)];

                    Bfield[1] = (1 - t) * (1 - u) * (1 - v) * Byfield[linesArray->At(0)] +
                                t * (1 - u) * (1 - v) * Byfield[linesArray->At(1)] +
                                t * u * (1 - v) * Byfield[linesArray->At(2)] + t * u * v * Byfield[linesArray->At(3)] +
                                (1 - t) * u * (1 - v) * Byfield[linesArray->At(4)] +
                                (1 - t) * u * v * Byfield[linesArray->At(5)] +
                                (1 - t) * (1 - u) * v * Byfield[linesArray->At(6)] +
                                t * (1 - u) * v * Byfield[linesArray->At(7)];

                    Bfield[2] = (1 - t) * (1 - u) * (1 - v) * Bzfield[linesArray->At(0)] +
                                t * (1 - u) * (1 - v) * Bzfield[linesArray->At(1)] +
                                t * u * (1 - v) * Bzfield[linesArray->At(2)] + t * u * v * Bzfield[linesArray->At(3)] +
                                (1 - t) * u * (1 - v) * Bzfield[linesArray->At(4)] +
                                (1 - t) * u * v * Bzfield[linesArray->At(5)] +
                                (1 - t) * (1 - u) * v * Bzfield[linesArray->At(6)] +
                                t * (1 - u) * v * Bzfield[linesArray->At(7)];
                }

            } //! returnValue

            else if (returnValue == 1)
            {
                cout << "-I- R3BFieldMap Point "
                     << " is just in one grid point!" << endl;

                Bfield[0] = Bxfield[linesArray->At(0)];
                Bfield[1] = Byfield[linesArray->At(0)];
                Bfield[2] = Bzfield[linesArray->At(0)];
            } // returnValue ==1

            else
            {
                cout << "-E- R3BFieldMap::GetFieldValue(): Point "
                     << ", but return value from GetLinesArrayForPosition() not valid!!" << endl;

                Bfield[0] = 0;
                Bfield[1] = 0;
                Bfield[2] = 0;

            } // whatever other cases
        }     //! outside of field area

        else
        {
            // cout << "-E- in R3BFieldMap::GetFieldValue(): Point "
            //       << " is outside the map field!" << endl;

            Bfield[0] = 0;
            Bfield[1] = 0;
            Bfield[2] = 0;
        }
        // Reset heap memory

        if (linesArray)
        {
            delete linesArray;
            linesArray = NULL;
        }

        // linesArray
        // localPoint;
    }
    else if (typeField == 2)
    {
        // cout << "-I- R3BFieldMap typeField 2"<< endl;
        Bfield[0] = 0.;
        Bfield[1] = -1. * 10.; // kGauss
        Bfield[2] = 0.;
    }

    // return the values
    bField[0] = Bfield[0] * fScale;
    bField[1] = Bfield[1] * fScale;
    bField[2] = Bfield[2] * fScale;
}

// ------------   Constructor from R3BFieldPar   --------------------------
R3BFieldMap::R3BFieldMap(R3BFieldPar* fieldPar)
{

    /*
     fType = 1;
     fPosX  = fPosY  = fPosZ  = 0.;
     fXmin  = fYmin  = fZmin  = 0.;
     fXmax  = fYmax  = fZmax  = 0.;
     fXstep = fYstep = fZstep = 0.;
     fNx    = fNy    = fNz    = 0;
     fScale = 1.;
     fBx    = fBy    = fBz    = NULL;
     if ( ! fieldPar ) {
       cerr << "-W- R3BFieldConst::R3BFieldMap: empty parameter container!"
        << endl;
       fName     = "";
       fFileName = "";
       fType     = 1;
     }
     else {
       fieldPar->MapName(fName);
       fPosX  = fieldPar->GetPositionX();
       fPosY  = fieldPar->GetPositionY();
       fPosZ  = fieldPar->GetPositionZ();
       fScale = fieldPar->GetScale();
       TString dir = getenv("VMCWORKDIR");
       fFileName = dir + "/input/" + fName + ".root";
       fType = fieldPar->GetType();
     }
   */
}
// ------------------------------------------------------------------------

// ------------   Destructor   --------------------------------------------
R3BFieldMap::~R3BFieldMap() {}
// ------------------------------------------------------------------------

// -----------   Intialisation   ------------------------------------------
void R3BFieldMap::Init()
{
    /*
      if      (fFileName.EndsWith(".root")) ReadRootFile(fFileName, fName);
      else if (fFileName.EndsWith(".dat"))  ReadAsciiFile(fFileName);
      else {
        cerr << "-E- R3BFieldMap::Init: No proper file name defined! ("
         << fFileName << ")" << endl;
         LOG(fatal) << "Init: No proper file name";
        }
    */
}

Int_t R3BFieldMap::GetLinesArrayForPosition(TVector3* pos, TArrayI* lines)
{
    //
    TVector3* posAux = new TVector3();
    Int_t tmp = GetLineForPosition(pos);
    lines->AddAt(tmp, 0);
    Int_t tmpPos = lines->At(0);
    GetPositionForLine(tmpPos, posAux);
    if ((*posAux) == (*pos))
    {
        delete posAux;
        return 1;
    }

    // Faster method
    lines->AddAt(lines->At(0) + stepsInY * stepsInZ, 1);
    lines->AddAt(lines->At(1) + stepsInZ, 2);
    lines->AddAt(lines->At(2) + 1, 3);
    lines->AddAt(lines->At(0) + stepsInZ, 4);
    lines->AddAt(lines->At(4) + 1, 5);
    lines->AddAt(lines->At(0) + 1, 6);
    lines->AddAt(lines->At(1) + 1, 7);

    if (posAux)
        delete posAux;
    return 0;
}

Int_t R3BFieldMap::GetLineForPosition(TVector3* pos) const
{
    //
    // Returns the line of the grid point with coordinates
    // equal or (closer and) below the coordinates of "pos"
    //

    Int_t elementX = (Int_t)((pos->X() - initialX) / gridStep);
    Int_t elementY = (Int_t)((pos->Y() - initialY) / gridStep);
    Int_t elementZ = (Int_t)((pos->Z() - initialZ) / gridStep);

    return (elementX * stepsInY * stepsInZ + elementY * stepsInZ + elementZ);
}

Int_t R3BFieldMap::GetPositionForLine(Int_t line, TVector3* pos) const
{
    //
    // Fills the position for the given line in the corresponding field map.
    // Return 1 if the given line is outside the field map list
    //
    if ((line > 15146 || line < 0) && typeField == 0)
        return 1; // ALADIN
    if ((line > 15146 || line < 0) && typeField == 3)
        return 1; // ALADIN_INVERSE
    if ((line > 420172 || line < 0) && typeField == 1)
        return 1; // R3B

    pos->SetXYZ(initialX + (gridStep * (Int_t)(line / (stepsInY * stepsInZ))),
                initialY + (gridStep * (Int_t)(((Int_t)(line / stepsInZ)) % stepsInY)),
                initialZ + (gridStep * (Int_t)(line % stepsInZ)));
    return 0;
}

// ----------   Write the map to an ASCII file   --------------------------
void R3BFieldMap::WriteAsciiFile(const char* fileName)
{

    // Open file
    cout << "-I- R3BFieldMap: Writing field map to ASCII file " << fileName << endl;
    ofstream mapFile(fileName);
    if (!mapFile.is_open())
    {
        cerr << "-E- R3BFieldMap:ReadAsciiFile: Could not open file! " << endl;
        return;
    }
}
// ------------------------------------------------------------------------

// -------   Write field map to a ROOT file   -----------------------------
void R3BFieldMap::WriteRootFile(const char* fileName, const char* mapName) {}
// ------------------------------------------------------------------------

// -----  Set the position of the field centre in global coordinates  -----
void R3BFieldMap::SetPosition(Double_t x, Double_t y, Double_t z) {}
// ------------------------------------------------------------------------
// ---------   Screen output   --------------------------------------------
void R3BFieldMap::Print(Option_t* option) const {}
// ------------------------------------------------------------------------
// ---------    Reset parameters and data (private)  ----------------------
void R3BFieldMap::Reset() {}
// ------------------------------------------------------------------------
// -----   Read field map from ASCII file (private)   ---------------------
void R3BFieldMap::ReadAsciiFile(const char* fileName) {}
// ------------------------------------------------------------------------
// -------------   Read field map from ROOT file (private)  ---------------
void R3BFieldMap::ReadRootFile(const char* fileName, const char* mapName) {}

ClassImp(R3BFieldMap)
