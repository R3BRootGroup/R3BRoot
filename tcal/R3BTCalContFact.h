/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
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

#include <FairContFact.h>

#include <Rtypes.h>
#include <TString.h>
#include <string.h>
#include <vector>

class FairParSet;

class R3BTCalContFact : public FairContFact
{
  public:
    /**
     * Default constructor.
     */
    R3BTCalContFact();

    /**
     * Destructor.
     */
    virtual ~R3BTCalContFact() = default;

    /**
     * Method to create a parameter container.
     * Called by FairRuntimeDB singleton class.
     * @param c a parameter container.
     * @return an instance of created parameter set.
     */
    FairParSet* createContainer(FairContainer* c) override;

  private:
    std::vector<std::string> containerNames;
    /**
     * Method to add a standard container
     */
    void addContainer(TString, TString);

    /**
     * Method to add a Fiber detector to the container
     */
#define ADD_FIBER(Name, NAME)                                                                       \
    do                                                                                              \
    {                                                                                               \
        addContainer(#Name "MAPMTTCalPar", #NAME " MAPMT TCAL Calibration Parameters");             \
        addContainer(#Name "SPMTTCalPar", #NAME " SPMT TCAL Calibration Parameters");               \
        addContainer(#Name "MAPMTTrigTCalPar", #NAME " MAPMT Trigger TCAL Calibration Parameters"); \
    } while (0)

    /**
     * Method to push a Fiber detector to the container
     */
#define PUSH_FIBER(Name)                                    \
    do                                                      \
    {                                                       \
        containerNames.push_back(#Name "MAPMTTCalPar");     \
        containerNames.push_back(#Name "SPMTTCalPar");      \
        containerNames.push_back(#Name "MAPMTTrigTCalPar"); \
    } while (0)

    /**
     * Method to specify a list of parameter containers managed by this factory.
     * Support for additional detrectors has to be implemented in this function.
     */
    void setAllContainers();

  public:
    ClassDefOverride(R3BTCalContFact, 2);
};
