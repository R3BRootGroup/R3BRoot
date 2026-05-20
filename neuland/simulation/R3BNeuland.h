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

#pragma once

#include "R3BNeulandPoint.h"
#include "TLorentzVector.h"
#include <FairDetector.h>
#include <R3BIOConnector.h>
#include <R3BNeulandCommon.h>
#include <R3BNeulandGeoCreator.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TClonesArray.h>
#include <TGeoMatrix.h>
#include <TString.h>
#include <map>
#include <memory>
#include <string>

class FairVolume;
class TClonesArray;
class R3BNeulandGeoPar;

/**
 * \brief NeuLAND detector simulation class
 * @author Jan Mayer, Yanzhao Wang
 * @since 12.01.2016
 *
 * For each simulated event, a vector data with name "NeulandPoints" is filled:
 * - NeulandPoints (R3BNeulandPoint), each representing energy deposition and light yield of a track in a paddle.
 *
 * Suitable geometry files require proper naming of the active volume (see CheckIfSensitive()) and copy numbers.
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

    /**
     * \brief Enable the automatic geometry build for NeuLAND.
     *
     * Automatically build the NeuLAND geometry without a geometry file. The geometry build is done in the class
     * R3B::Neuland::Geometry::Creator. If this is disabled, an geometry file corresponding to the number of double
     * plane must be present.
     * @param is_enabled Whether the geometry build should be enabled or not.
     */
    void EnableAutoGeoBuild(bool is_enabled = true) { is_geo_auto_built = is_enabled; }

    /**
     * \brief Set the translation and rotation of the NeuLAND detector
     *
     * Set the translation and rotation of the NeuLAND detector using parameter with the type `TGeoCombiTrans`. This
     * will override the value specified by the constructors.
     * @param pos Input translation and rotation matrix.
     * @see <a href="https://root.cern/doc/master/classTGeoCombiTrans.html">TGeoCombiTrans</a>
     */
    void SetLocationDirection(const TGeoCombiTrans& pos) { rot_trans_ = pos; }

  private:
    bool is_geo_auto_built = false;                       //!< Flag to check if geo-build is needed.
    bool is_last_hit_done_ = false;                       //!< Flag to check if last hit finished.
    int num_of_planes_ = R3B::Neuland::MaxNumberOfPlanes; //!< The number of planes.

    // Track information to be stored until the track leaves the active volume.
    int track_id_ = 0;           //!< A buffer for the track IDs.
    int paddle_id_ = 0;          //!< A buffer for the paddle ids.
    int particle_id_ = 0;        //!< A buffer for the particle IDs.
    int parent_particle_id_ = 0; //!< A buffer for the parent particle IDs.
    double time_ = 0.;           //!< A buffer for time values.
    double length_ = 0.;         //!< A buffer for track lengths.
    double energy_loss_ = 0.;    //!< A buffer for the energy losses.
    double light_yield_ = 0.;    //!< A buffer for the light yields.
    TLorentzVector pos_in_;      //!< A buffer for the position where the track goes into the volume.
    TLorentzVector pos_out_;     //!< A buffer for the position where the track goes out of the volume.
    TLorentzVector mom_in_;      //!< A buffer for the momentum when the track goes into the volume.
    TLorentzVector mom_out_;     //!< A buffer for the position when the track goes out of the volume.

    /// The TCA data buffer used to be returned by virtual method GetCollection(int iColl).
    std::unique_ptr<TClonesArray> tca_points_buffer_ = std::make_unique<TClonesArray>(R3BNeulandPoint::Class()); //!
    /// Output data written to the ROOT file.
    R3B::OutputVectorConnector<R3BNeulandPoint> neuland_points_{ "NeulandPoints" }; //!
    /// Output parameter containing the geometry information.
    R3BNeulandGeoPar* neuland_geo_par_ = nullptr; //!
    /// Creating NeuLAND geometry if EnableAutoGeoBuild() is enabled.
    R3B::Neuland::Geometry::Creator geo_creator_; //!
    /// Rotation and translation of NeuLAND detector.
    TGeoCombiTrans rot_trans_;
    /// Geometry file name. The file must be present if EnableAutoGeoBuild() is disabled
    std::string geo_file_;
    /// A map with the track ID as the key and the particle ID as the value. Used to find the parent particle ID of the
    /// current track,
    std::map<int, int> track_pid_map_;

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
