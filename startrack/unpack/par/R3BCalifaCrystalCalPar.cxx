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

#include "R3BCalifaCrystalCalPar.h"

#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h" // for FairDbOutRowStream
#include "FairDbStatement.h"      // for FairDbStatement

#include "FairParamList.h" // for FairParamList

#include "FairLogger.h"

#include "Riosfwd.h" // for ostream
#include "TArrayD.h" // for TString
#include "TString.h" // for TString

#include <memory> // for auto_ptr, etc
#include <sstream>
#include <stdio.h>  // for exit
#include <stdlib.h> // for exit
#include <vector>   // for vector, vector<>::iterator

using namespace std;

ClassImp(R3BCalifaCrystalCalPar);

R3BCalifaCrystalCalPar::R3BCalifaCrystalCalPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairDbObjTableMap(name, title, context, own)
{
    fDUCalParams = new TObjArray(500);
}

R3BCalifaCrystalCalPar::~R3BCalifaCrystalCalPar()
{
    if (fDUCalParams)
    {
        delete fDUCalParams;
        fDUCalParams = NULL;
    }
}

void R3BCalifaCrystalCalPar::putParams(FairParamList* list)
{
    std::cout << "-I- R3BCalifaCrystalCalPar::putParams() called" << std::endl;
    if (!list)
    {
        return;
    }
    list->add("NrOfDUnits", fDUCalParams->GetEntries());

    for (Int_t i = 0; i < fDUCalParams->GetEntries(); i++)
    {
        stringstream ss;
        ss << i;
        TString du_id(ss.str());
        TArrayD values(8);
        R3BCalifaDUCalPar* dupar = (R3BCalifaDUCalPar*)fDUCalParams->At(i);
        values[0] = dupar->GetGammaCal_offset();
        values[1] = dupar->GetGammaCal_gain();
        values[2] = dupar->GetToTCal_par0();
        values[3] = dupar->GetToTCal_par1();
        values[4] = dupar->GetToTCal_par2();
        values[5] = dupar->GetRangeCal_offset();
        values[6] = dupar->GetRangeCal_gain();
        values[7] = dupar->GetQuenchingFactor();
        values[8] = dupar->GetPidGain();

        list->add(du_id.Data(), values);
    }
}

Bool_t R3BCalifaCrystalCalPar::getParams(FairParamList* list)
{
    if (!list)
    {
        LOG(warn) << "R3BCalifaCrystalCalPar::getParams(): list is NULL";
        return kFALSE;
    }
    Int_t ndus;
    if (!list->fill("NrOfDUnits", &ndus))
    {
        LOG(warn) << "R3BCalifaCrystalCalPar::getParams(): Could not read NrOfDUnits from parameter list";
        return kFALSE;
    }

    LOG(debug) << "R3BCalifaCrystalCalPar::getParams(): NrOFUNits " << ndus;

    TArrayD* values = new TArrayD(9);

    for (Int_t i = 0; i < ndus; i++)
    {
        stringstream ss;
        ss << i;
        TString du_id(ss.str());
        if (!list->fill(du_id.Data(), values))
            return kFALSE;

        R3BCalifaDUCalPar* dupar = new R3BCalifaDUCalPar();
        dupar->SetDetectionUnit(i);
        dupar->SetGammaCal_offset(values->At(0));
        dupar->SetGammaCal_gain(values->At(1));
        dupar->SetToTCal_par0(values->At(2));
        dupar->SetToTCal_par1(values->At(3));
        dupar->SetToTCal_par2(values->At(4));
        dupar->SetRangeCal_offset(values->At(5));
        dupar->SetRangeCal_gain(values->At(6));
        dupar->SetQuenchingFactor(values->At(7));
        dupar->SetPidGain(values->At(8));
        fDUCalParams->Add(dupar);
        cout << " Entries in Array " << fDUCalParams->GetEntries() << endl;
        for (Int_t j = 0; j < 8; j++)
            cout << " idx: " << j << "values# " << values->At(j) << endl;
    }

    if (values)
        delete values;

    return kTRUE;
}

void R3BCalifaCrystalCalPar::ReadFile(string file)
{
    vector<string> data;
    string datasegments, line;

    ifstream infile(file.c_str());
    while (getline(infile, line))
    {
        stringstream dataline;
        dataline << line;
        while (getline(dataline, datasegments, '\t'))
        {
            if (datasegments == "null")
                data.push_back("");
            else
                data.push_back(datasegments);
        }
        dataline.clear();
        R3BCalifaDUCalPar dupar;
        dupar.SetDetectionUnit(atoi(data[0].c_str()));
        dupar.SetGammaCal_offset(atof(data[1].c_str()));
        dupar.SetGammaCal_gain(atof(data[2].c_str()));
        dupar.SetToTCal_par0(atof(data[3].c_str()));
        dupar.SetToTCal_par1(atof(data[4].c_str()));
        dupar.SetToTCal_par2(atof(data[5].c_str()));
        dupar.SetRangeCal_offset(atof(data[6].c_str()));
        dupar.SetRangeCal_gain(atof(data[7].c_str()));
        dupar.SetQuenchingFactor(atof(data[8].c_str()));
        dupar.SetPidGain(atof(data[9].c_str()));
        fDUCalParams->Add(&dupar);
    }
}

void R3BCalifaCrystalCalPar::clear() {}

void R3BCalifaCrystalCalPar::fill(UInt_t rid)
{
    // Fill the lists with correspondin TimeStamps (runID)
    cout << "-I- R3BCalifaCrystalCalPar::fill() called with RID# " << rid << endl;

    R3BCalifaDUCalPar tpar;

    FairDbReader<R3BCalifaDUCalPar>* r_tpar = tpar.GetParamReader();

    // Define a Global Context
    ValTimeStamp ts(rid);
    time_t shift = ts.GetSec() + 60;
    ValTimeStamp end(shift, 0);
    cout << " DATE of RETRIEVAL " << end.AsString("s") << endl;
    ValCondition context(FairDbDetector::kLand, DataType::kData, end);

    // Activate reading for this Context
    r_tpar->Activate(context, GetVersion());

    Int_t numTCh = r_tpar->GetNumRows();
    cout << "-I- R3BCalifaCrystalCalPar numOfRow " << numTCh << endl;
    for (int i = 0; i <= numTCh; ++i)
    {
        R3BCalifaDUCalPar* tcal_par = (R3BCalifaDUCalPar*)r_tpar->GetRow(i);
        if (!tcal_par)
        {
            continue;
        }
        else
            fDUCalParams->Add(tcal_par);
    }
    cout << "-I- R3BCalifaCrystalCalPar filled with  " << fDUCalParams->GetEntries() << " Cal Objects " << endl;
}

void R3BCalifaCrystalCalPar::store(UInt_t rid)
{
    //<DB> store the lists
    Int_t nParams = fDUCalParams->GetEntries();

    cout << "-I- R3BCalifaCrystalCalPar::store() called with RID# " << rid << " nParams: " << nParams << endl;

    // Crystal nodes stored row-wized
    R3BCalifaDUCalPar iPar;
    FairDbWriter<R3BCalifaDUCalPar>* cW = iPar.ActivateWriter(rid);

    if (cW)
    {
        // TCal Objects
        for (Int_t i = 0; i < nParams; i++)
        {
            R3BCalifaDUCalPar* t_par = (R3BCalifaDUCalPar*)fDUCalParams->At(i);
            if (t_par)
                *cW << *t_par;
        }
        // Reset the Writer
        if (!cW->Close())
        {
            cout << " -W- R3BCalifaCrystalCalPar:: IO Error writing crystal data node ******** " << endl;
        }
    } //! (cWriter)
    else
    {
        cout << " -W- R3BCalifaCrystalCalPar:: Writer for R3BCalifaCrystalPar not created " << endl;
    }
}

void R3BCalifaCrystalCalPar::Print()
{

    std::cout << " -----------  Califa Detection Unit Calib. Parameters -------------  " << std::endl;

    std::cout << " Number of DUCal Parameters " << fDUCalParams->GetEntries() << std::endl;
    for (Int_t i = 0; i < fDUCalParams->GetEntries(); i++)
    {
        R3BCalifaDUCalPar* du_par = (R3BCalifaDUCalPar*)fDUCalParams->At(i);
        cout << "----------------------------------------------------------------------" << endl;
        if (du_par)
            du_par->Print();
    }
}
