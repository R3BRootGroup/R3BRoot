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

/**
R3BKeepEventProbabilityMfi.h

This class reads the survival probability for an event/hit in a fiber of MFI from a file and save it to a global array.

Testing phase! Please report bugs to P.Schrock@gsi.de
*/

#ifndef R3BKEEPEVENTPROBABILITYMFI_H
#define R3BKEEPEVENTPROBABILITYMFI_H 1

#include "TMath.h"

#include <string>

class R3BKeepEventProbabilityMfi
{

  public:
    /**
    Default constructor. No input file. All fibers will get an efficiency of 100%.
    */
    R3BKeepEventProbabilityMfi();

    /**
    Special constructor getting a filename as string and starts immediately the ReadProbabilityFromFile function.
    */
    R3BKeepEventProbabilityMfi(std::string _filename);

    /**
    Defauld destructor.
    */
    ~R3BKeepEventProbabilityMfi();

    /**
    Saves survival probability for a hit in a fiber to a global array.
    */
    void SetProbability(Int_t fiber, Double_t prob);

    /**
    Return the survival probability for a hit in a fiber.
    */
    Double_t GetProbability(Int_t fiber);

    /**
    Reads the survival probability for a hit in a fiber from an external text file.

    Note: the file is the same used as input for ggland, hence the format is fix!
    One line looks like 'tree-keep-event-prob=1:0.000000,' with '1' the fiber number (starts counting from 1) and
    '0.000000' the event/hit survival probability, followed by a ','. Everything before the '=' will be deleted by this
    function. Comment lines are allowed, starting with '//'.

    todo: add global efficiency as entry '0', if necessary.
    */
    void ReadProbabilityFromFile(std::string filename);

  protected:
    // private:

    /**
    Survival probability for a hit in a fiber. Fibers start counting from '1'!
    */
    Double_t fKeepEventProb[1025];
};

#endif
