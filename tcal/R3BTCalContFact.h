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

#ifndef R3BTCALCONTFACT_H
#define R3BTCALCONTFACT_H

#include "FairContFact.h"

#include "Rtypes.h"

class FairParSet;

/**
 * Factory class for all TCAL parameter containers.
 * Currently supported:
 * NeuLAND - LandTCalPar
 * LOS - LosTCalPar
 * @author D. Kresan
 * @since September 3, 2015
 */
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
    ~R3BTCalContFact() {}

    /**
     * Method to create a parameter container.
     * Called by FairRuntimeDB singleton class.
     * @param c a parameter container.
     * @return an instance of created parameter set.
     */
    FairParSet* createContainer(FairContainer* c);

  private:
    /**
     * Method to add a standard container
     */
    void addContainer(TString, TString);
    /**
     * Method to specify a list of parameter containers managed by this factory.
     * Support for additional detrectors has to be implemented in this function.
     */
    void setAllContainers();

    ClassDef(R3BTCalContFact, 1);
};

#endif /* !R3BTCALCONTFACT_H */
