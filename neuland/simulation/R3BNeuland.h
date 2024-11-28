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
#include "R3BNeulandPoint.h"
#include "TLorentzVector.h"
#include <R3BIOConnector.h>
#include <R3BNeulandCommon.h>
#include <R3BNeulandGeoCreator.h>
#include <memory>
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

class R3BNeuland : public FairDetector
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

    void EnableAutoGeoBuild(bool is_enabled = true) { is_geo_auto_built = is_enabled; }

    void SetLocationDirection(const TGeoCombiTrans& pos) { rot_trans_ = pos; }

  private:
    bool is_geo_auto_built = false;
    int num_of_planes_ = R3B::Neuland::MaxNumberOfPlanes;
    R3B::OutputVectorConnector<R3BNeulandPoint> neuland_points_{ "NeulandPoints" }; //!
    std::unique_ptr<TClonesArray> tca_points_buffer_ = std::make_unique<TClonesArray>(R3BNeulandPoint::Class());
    R3BNeulandGeoPar* neuland_geo_par_ = nullptr; //!
    R3B::Neuland::Geometry::Creator geo_creator_; //!
    TGeoCombiTrans rot_trans_;
    std::string geo_file_;
    std::map<int, int> track_pid_map_;

    /** Track information to be stored until the track leaves the active volume. */
    int fTrackId = 0;
    int fPaddleId = 0;
    TLorentzVector pos_in_;
    TLorentzVector pos_out_;
    TLorentzVector mom_in_;
    TLorentzVector mom_out_;
    double time_ = 0.;
    double length_ = 0.;
    double energy_loss_ = 0.;
    double light_yield_ = 0.;
    bool is_last_hit_done_ = false;
    int particle_id_ = 0;
    int parent_particle_id_ = 0;

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

    void ConstructGeometry() override;

    void Initialize() override;

    // non-virtual functions:

    void create_geo();

    void create_geo_from_root_file();

    ClassDefOverride(R3BNeuland, 4);
};

#endif // R3BNEULAND_H
