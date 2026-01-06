/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// --------------------------------------------------------------
// -----             R3BAlpideMappingPar source file        -----
// -----      Created 11/02/22 by J.L. Rodriguez-Sanchez    -----
// --------------------------------------------------------------

#pragma once

#include <FairParGenericSet.h>

#include <Rtypes.h>
#include <cstdint>
#include <stdint.h>
#include <vector>

#define DAlpideRows 512
#define DAlpideCols 1024

class FairParamList;

class R3BAlpideMappingPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BAlpideMappingPar(const char* name = "alpideMappingPar",
                        const char* title = "ALPIDE Mapping Parameters",
                        const char* context = "AlpideMappingParContext");

    /** Destructor **/
    virtual ~R3BAlpideMappingPar();

    /** Method to reset all parameters **/
    void clear() override;

    /** Method to store all parameters using FairRuntimeDB **/
    void putParams(FairParamList* list) override;

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list) override;

    /** Method to print values of parameters to the standard output **/
    void print() override;
    void printParams() override;

    /** Accessor functions **/
    [[nodiscard]] int GetNbSensors() const { return fNbSensors; }
    [[nodiscard]] int GetGeoVersion() const { return fGeoVersion; }
    [[nodiscard]] int GetNbMaskPixel() const { return fNbMaskPixels; }

    // [SENSOR][ROW][COL]
    //    4      3    4  dígits
    [[nodiscard]] uint32_t GetRow(uint64_t id) { return (id / 10'000) % 1'000; }
    [[nodiscard]] uint32_t GetCol(uint64_t id) { return id % 10'000; }
    [[nodiscard]] uint32_t GetSensorId(uint64_t id) { return id / 10'000'000; }
    [[nodiscard]] uint64_t GetMaskPixel(int nbpixel) const { return fMask_sensors[nbpixel]; }

    void SetNbSensors(int n) { fNbSensors = n; }
    void SetGeoVersion(int v) { fGeoVersion = v; }
    void SetMaskPixel(uint64_t maskid)
    {
        fMask_sensors.push_back(maskid);
        fNbMaskPixels = fMask_sensors.size();
    }

  private:
    int fNbSensors = 363;
    int fNbMaskPixels = 0;
    int fGeoVersion = 2024;
    int fAlpideCols = DAlpideCols;
    int fAlpideRows = DAlpideRows;
    std::vector<uint64_t> fMask_sensors;

    const R3BAlpideMappingPar& operator=(const R3BAlpideMappingPar&); /*< an assignment operator>*/
    R3BAlpideMappingPar(const R3BAlpideMappingPar&);                  /*< a copy constructor >*/

  public:
    ClassDefOverride(R3BAlpideMappingPar, 2); // NOLINT
};
