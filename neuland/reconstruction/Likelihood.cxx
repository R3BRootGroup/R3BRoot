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

#include "Likelihood.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <vector>

Neuland::Likelihood::Likelihood(const std::string& filename)
    : fFileName(filename)
{

    std::cout << "Neuland::Likelihood::Likelihood processing " << filename << std::endl;
    std::ifstream file(filename);

    std::string line;
    std::vector<std::string> split;

    // Get Hypotheses
    std::getline(file, line);
    boost::algorithm::split(split, line, boost::is_any_of("\t "), boost::algorithm::token_compress_on);

    std::vector<H> hs;
    for (size_t i = 1; i < split.size(); i++)
    {
        hs.push_back(std::stoi(split.at(i)));
    }

    // Get Data
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        boost::algorithm::split(split, line, boost::is_any_of("\t "), boost::algorithm::token_compress_on);
        for (size_t i = 1; i < split.size(); i++)
        {
            fData[std::stoi(split.at(0))][hs.at(i - 1)] = std::stof((split.at(i)));
        }
    }
}

double Neuland::Likelihood::P(Neuland::Likelihood::E e, Neuland::Likelihood::H h) const
{
    const auto eit = fData.lower_bound(e);
    if (eit == fData.end())
    {
        std::cout << "Neuland::Likelihood::P(E|H) - E " << e << " not found in " << fFileName << std::endl;
        return 0;
    }
    const auto nit = eit->second.find(h);
    if (nit == eit->second.end())
    {
        std::cout << "Neuland::Likelihood::P(E|H) - H " << h << " not found in " << fFileName << std::endl;
        return 0;
    }
    return nit->second;
}
