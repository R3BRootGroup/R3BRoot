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
 
#ifndef R3BLogger_H
#define R3BLogger_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "FairLogger.h"

class R3BLogger : public FairLogger
{
  public:
    R3BLogger();
    
   private:
     ~R3BLogger();

  public:
    ClassDefOverride(R3BLogger, 0)
};

#define R3BLOG(severity) \
    std::string fileName_(__FILE__);\
    std::string fileName2_(fileName_.substr(fileName_.find_last_of("/")+1));\
    std::stringstream ss_;\
    ss_ << fileName2_ << "::" << __FUNCTION__ << "()[" << __LINE__ << "]: ";\
    LOG(severity) << ss_.str()\
    
#define R3BLOG_IF(severity, condition) \
    std::string fileName_(__FILE__);\
    std::string fileName2_(fileName_.substr(fileName_.find_last_of("/")+1));\
    std::stringstream ss_;\
    ss_ << fileName2_ << "::" << __FUNCTION__ << "()[" << __LINE__ << "]: ";\
    LOG_IF(severity, condition) << ss_.str()\

#endif // R3BLogger_H
