/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
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

#ifndef R3BCALIFAPOINT_H
#define R3BCALIFAPOINT_H 1

#include <FairMCPoint.h>
#include <TObject.h>
#include <TVector3.h>

class R3BCalifaPoint : public FairMCPoint
{
  public:
    /** Default constructor **/
    R3BCalifaPoint();

    /** Constructor with arguments
     *@param trackID  Index of MCTrack
     *@param detID    Detector ID
     *@param trackPID Particle PID
     *@param ident    Crystal ID
     *@param posIn    Ccoordinates at entrance to active volume [cm]
     *@param momIn    Momentum of track at entrance [GeV]
     *@param tof      Time since event start [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/
    R3BCalifaPoint(int trackID,
                   int detID,
                   int trackPID,
                   int ident,
                   TVector3 posIn,
                   TVector3 momIn,
                   double tof,
                   double length,
                   double eLoss);

    /** Copy constructor **/
    R3BCalifaPoint(const R3BCalifaPoint&);

    R3BCalifaPoint& operator=(const R3BCalifaPoint&) { return *this; }

    /** Destructor **/
    virtual ~R3BCalifaPoint();

    /** Accessors **/
    int GetCrystalId() const { return fCrystalId; }
    int GetTrackPid() const { return fTrackPID; }
    double GetXIn() const { return fX; }
    double GetYIn() const { return fY; }
    double GetZIn() const { return fZ; }
    void PositionIn(TVector3& pos) { pos.SetXYZ(fX, fY, fZ); }

  protected:
    int fCrystalId; ///< crystal index
    int fTrackPID;

    ClassDef(R3BCalifaPoint, 2)
};

#endif
