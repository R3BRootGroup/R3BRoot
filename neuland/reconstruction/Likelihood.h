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

#ifndef R3BROOT_LIKELIHOOD_H
#define R3BROOT_LIKELIHOOD_H

#include <map>
#include <string>

namespace Neuland
{
    class Likelihood
    {
      public:
        using E = double;
        using H = int;

        explicit Likelihood(const std::string& filename);

        double P(E e, H h) const;

      private:
        std::string fFileName;
        std::map<E, std::map<H, double>> fData;
    };
} // namespace Neuland

#endif // R3BROOT_LIKELIHOOD_H
