/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------
// -----                                                       -----
// -----                R3BMusicMapped2CalPar                  -----
// -----        Created 29/01/20 by J.L. Rodriguez-Sanchez     -----
// -----------------------------------------------------------------

#ifndef R3BMusicMapped2CalPar_H
#define R3BMusicMapped2CalPar_H 1

#include "FairTask.h"
#include "R3BMusicMapped2Cal.h"
#include "R3BMusicMappedData.h"
#include "TGraph.h"
#include "TH1F.h"

class TClonesArray;
class R3BTDCCyclicCorrector;
class R3BMusicCalPar;

class R3BMusicMapped2CalPar : public FairTask
{
  public:
    /** Default constructor **/
    R3BMusicMapped2CalPar();

    /** Standard constructor **/
    R3BMusicMapped2CalPar(const TString& name,
                          int iVerbose = 1,
                          const TString& namedeta = "Mwpc0",
                          const TString& namedetb = "Mwpc2");

    /** Destructor **/
    virtual ~R3BMusicMapped2CalPar();

    /** Virtual method Exec **/
    virtual void Exec(Option_t*);

    /** Virtual method FinishTask **/
    virtual void FinishTask();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Method to set up the position of DetA **/
    void SetPosDetA(float pos) { fPosDetA = pos; }

    /** Method to set up the position of DetB **/
    void SetPosDetB(float pos) { fPosDetB = pos; }

    /** Method to set up the position of Music **/
    void SetPosMusic(float pos) { fPosMusic = pos; }

    /** Method to set up the limits for fit **/
    void SetFitLimits(int left, int right)
    {
        fLimit_left = left;
        fLimit_right = right;
    }

  private:
    int fNumAnodes;
    int fMaxMult;
    int fMinStadistics;
    int fNumParams;
    int fNumPosParams;
    int fNumAnodesRef;
    int fLimit_left;
    int fLimit_right;
    int fMaxSigma;
    TArrayF* CalParams;
    TArrayF* PosParams;

    TString fNameDetA;
    float fPosDetA; // Position in the beam direction in mm
    TString fNameDetB;
    float fPosDetB;  // Position in the beam direction in mm
    float fPosMusic; // Position in the beam direction in mm

    int mulanode[MAX_NB_MUSICANODE + MAX_NB_MUSICTREF];
    double energy[MAX_MULT_MUSIC_CAL][MAX_NB_MUSICANODE + MAX_NB_MUSICTREF];
    double dtime[MAX_MULT_MUSIC_CAL][MAX_NB_MUSICANODE + MAX_NB_MUSICTREF];

    R3BTDCCyclicCorrector* fCyclicCorrector;
    R3BMusicCalPar* fCal_Par;         /**< Parameter container. >*/
    TClonesArray* fMusicMappedDataCA; /**< Array with Music Mapped-input data. >*/
    TClonesArray* fHitItemsDetA;      /**< Array with hit items. */
    TClonesArray* fHitItemsDetB;      /**< Array with hit items. */

    TGraph** fg_anode;

  public:
    // Class definition
    ClassDef(R3BMusicMapped2CalPar, 1)
};

#endif
