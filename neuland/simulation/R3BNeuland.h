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

#ifndef R3BNEULAND_H
#define R3BNEULAND_H

#include "R3BDetector.h"
#include "R3BIOConnector.h"
#include "R3BNeulandPoint.h"
#include "TLorentzVector.h"
#include <string>

class FairVolume;
class TClonesArray;
class R3BNeulandGeoPar;

/**
 * NeuLAND detector simulation class
 * @author Jan Mayer
 * @since 12.01.2016
 * For each simulated event, TClonesArrays are filled:
 * - NeulandPoints (R3BNeulandPoint), each representing energy deposition and light yield of a track in a paddle
 * Suitable geometry files require proper naming of the active volume (see CheckIfSensitive) and copy numbers.
 */

class R3BNeuland : public R3BDetector
{
  public:
    /** Default constructor */
    R3BNeuland();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation */
    R3BNeuland(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation */
    explicit R3BNeuland(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Standard constructor.
     *@param nDP     number of double planes
     *@param trans   position
     *@param rot     rotation */
    R3BNeuland(int nDP, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param nDP     number of double planes
     *@param combi   position + rotation */
    explicit R3BNeuland(int nDP, const TGeoCombiTrans& combi = TGeoCombiTrans());

  private:
    R3B::OutputVectorConnector<R3BNeulandPoint> fNeulandPoints{ "NeulandPoints" }; //!
    R3BNeulandGeoPar* fNeulandGeoPar = nullptr;                                    //!
    std::unique_ptr<TClonesArray> TCAPointsBuffer = std::make_unique<TClonesArray>(R3BNeulandPoint::Class());
    std::map<int, int> fTrackPidMap;

    /** Track information to be stored until the track leaves the active volume. */
    int fTrackId = 0;
    int fPaddleId = 0;
    TLorentzVector fPosIn;
    TLorentzVector fPosOut;
    TLorentzVector fMomIn;
    TLorentzVector fMomOut;
    double fTime = 0.;
    double fLength = 0.;
    double fEnergyLoss = 0.;
    double fLightYield = 0.;
    bool fIsLastHitDone = false;
    int fParticleId = 0;
    int fParentParticleId = 0;

    // private virtual functions:

    auto ProcessHits(FairVolume* /*v*/ = nullptr) -> bool override;

    void EndOfEvent() override;

    void FinishEvent() override;

    void Print(Option_t* /*unused*/ = "") const override;

    void Reset() override;

    auto CheckIfSensitive(std::string name) -> bool override;

    [[nodiscard]] auto GetCollection(int iColl) const -> TClonesArray* override;

    void Register() override;

    // private non-virtual member functions:

    void reset_values();
    void write_parameter_file();

    ClassDefOverride(R3BNeuland, 3);
};

#endif // R3BNEULAND_H
