/******************************************************************************
 *   Copyright (C) 2021 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2021-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------------
// -----                          R3BLogger                         -----
// -----             Created 15/12/21 by J.L. Rodriguez-Sanchez     -----
// ----------------------------------------------------------------------

#pragma once

#include <FairLogger.h>
#include <Rtypes.h>
#include <cstdlib>
#include <fairlogger/Logger.h>

#include <Rtypes.h>
#include <cstdlib>

class R3BLogger : public FairLogger
{
  public:
// NOLINTBEGIN
#define R3BLOG(severity, x)                                                                                    \
    do                                                                                                         \
    {                                                                                                          \
        std::string infile(__FILE__);                                                                          \
        std::stringstream ss;                                                                                  \
        ss << infile.substr(infile.find_last_of("/") + 1) << ":" << __LINE__ << ":" << __FUNCTION__ << "(): "; \
        LOG(severity) << ss.str() << x;                                                                        \
    } while (0)

#define R3BLOG_IF(severity, condition, x)                                                                          \
    do                                                                                                             \
    {                                                                                                              \
        if (condition)                                                                                             \
        {                                                                                                          \
            std::string infile(__FILE__);                                                                          \
            std::stringstream ss;                                                                                  \
            ss << infile.substr(infile.find_last_of("/") + 1) << ":" << __LINE__ << ":" << __FUNCTION__ << "(): "; \
            LOG(severity) << ss.str() << x;                                                                        \
        }                                                                                                          \
    } while (0)
    // NOLINTEND
  private:
    R3BLogger();
    ~R3BLogger() = default;

  public:
    ClassDefOverride(R3BLogger, 0);
};
