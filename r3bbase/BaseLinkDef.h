// clang-format off

/******************************************************************************
 *   Copyright (C) 2015 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2015-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class R3BModule+;
#pragma link C++ class R3BDetector+;
#pragma link C++ class R3BEventHeader+;
#pragma link C++ class R3BWhiterabbitPropagator+;
#pragma link C++ class R3BDataPropagator+;
#pragma link C++ class R3BFileSource+;
#pragma link C++ class R3BFileSource2+;
#pragma link C++ class R3BEventHeaderPropagator+;
#pragma link C++ class R3BLogger+;
#pragma link C++ class R3BTcutPar+;
#pragma link C++ class R3BTsplinePar+;
#pragma link C++ class R3BCoarseTimeStitch+;
#pragma link C++ class R3B::ParRootFileIo+;
#pragma link C++ class R3B::DetParRootFileIo+;
#pragma link C++ class R3B::ValueError<double>+;
#pragma link C++ class ROOT::Math::Cartesian3D<R3B::ValueErrorD>;
#pragma link C++ class ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<R3B::ValueErrorD>, ROOT::Math::DefaultCoordinateSystemTag>;
#pragma link C++ class pair<R3B::ValueError<double>, R3B::ValueError<double>>+;
#pragma link C++ class R3B::LRPair<R3B::ValueError<double>>+;
#pragma link C++ class R3B::LRPair<int>+;
#pragma link C++ class R3B::LRPair<double>+;
#pragma link C++ class R3B::LRPair<float>+;
#endif
