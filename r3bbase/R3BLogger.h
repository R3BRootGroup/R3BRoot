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

// ----------------------------------------------------------------------
// -----                          R3BLogger                         -----
// -----             Created 15/12/21 by J.L. Rodriguez-Sanchez     -----
// ----------------------------------------------------------------------

#ifndef R3BLogger_H
#define R3BLogger_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "FairLogger.h"

class R3BLogger : public FairLogger
{
  public:
    R3BLogger();

#define R3BLOG(severity, x)                                                                                          \
    if (true)                                                                                                        \
    {                                                                                                                \
        std::string fileName(__FILE__);                                                                              \
        std::stringstream ss;                                                                                        \
        ss << fileName.substr(fileName.find_last_of("/") + 1) << "::" << __FUNCTION__ << "()[" << __LINE__ << "]: "; \
        LOG(severity) << ss.str() << x;                                                                              \
    }                                                                                                                \
    else                                                                                                             \
        (void)0

#define R3BLOG_IF(severity, condition, x)                                                                        \
    if (true)                                                                                                    \
    {                                                                                                            \
        std::string fileNameif(__FILE__);                                                                        \
        std::stringstream ssif;                                                                                  \
        ssif << fileNameif.substr(fileNameif.find_last_of("/") + 1) << "::" << __FUNCTION__ << "()[" << __LINE__ \
             << "]: ";                                                                                           \
        LOG_IF(severity, condition) << ssif.str() << x;                                                          \
    }                                                                                                            \
    else                                                                                                         \
        (void)0

  private:
    ~R3BLogger();

  public:
    ClassDefOverride(R3BLogger, 0)
};

#endif // R3BLogger_H
