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

// -----------------------------------------------------------------------------
// -----                              R3BTrackData                             -----
// -----                                            l                   -----
// -----------------------------------------------------------------------------

#ifndef R3BTRACKDATA_H
#define R3BTRACKDATA_H

#include "R3BTrack.h"
#include "TObject.h"

class R3BTrackData : public R3BTrack
{
  public:
    /** Default constructor **/
    R3BTrackData();
    
    /** Constructor with arguments **/
    R3BTrackData(Double_t x, Double_t y, Double_t z, Double_t px, Double_t py, Double_t pz, Int_t q, Double_t AoZ, Double_t chix, Double_t chiy, Int_t quality, std::vector<Double_t> detPos, Double_t t);
    
     /** Copy constructor **/
    R3BTrackData(const R3BTrackData&);

    R3BTrackData& operator=(const R3BTrackData&) { return *this; }

    /** Destructor **/   
    virtual ~R3BTrackData();
   
   /** Accessors **/
     Double_t GetTime() const { return fTime; }
    
    std::vector<Double_t> GetDetPos() const {return fDetPos; }
    void SetDetPos(std::vector<Double_t> list) {fDetPos = std::move(list);}
    
  protected:
    std::vector<Double_t> fDetPos; // List with detectors positions and tofd_time. 
    Double_t fTime;

  public:
    ClassDef(R3BTrackData, 1)
};

#endif
