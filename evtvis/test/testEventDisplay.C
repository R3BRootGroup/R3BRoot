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

void testEventDisplay()
{
    auto fRun = new FairRunAna();
    fRun->SetSource(new FairFileSource("test.simu.root"));
    fRun->SetSink(new FairRootFileSink("event.test.root"));

    auto rtdb = fRun->GetRuntimeDb();
    auto parIo1 = new FairParRootFileIo();
    parIo1->open("test.para.root");
    rtdb->setFirstInput(parIo1);
    rtdb->print();

    auto fMan = new R3BEventManager();
    auto Track = new R3BMCTracks("Monte-Carlo Tracks");

    fMan->AddTask(Track);
    // fMan->Init();

    std::cout << "Macro finished successfully." << std::endl;
}
