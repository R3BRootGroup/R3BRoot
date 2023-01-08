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

#include "R3BKeepEventProbabilityMfi.h"

#include "TMath.h"

#include <fstream>
#include <iostream>
#include <sstream> //for istringstrem

#include <string>

// using std::cout;
// using std::endl;
using namespace std;

R3BKeepEventProbabilityMfi::R3BKeepEventProbabilityMfi()
{
    // set standard value for event/hit survival probability to 100%
    for (Int_t ii = 0; ii < 1025; ii++)
    {
        SetProbability(ii, 1.0);
    }
}

R3BKeepEventProbabilityMfi::R3BKeepEventProbabilityMfi(std::string _filename) { ReadProbabilityFromFile(_filename); }

R3BKeepEventProbabilityMfi::~R3BKeepEventProbabilityMfi() {}

void R3BKeepEventProbabilityMfi::SetProbability(Int_t fiber, Double_t prob)
{
    fKeepEventProb[fiber] = prob;

    // 	cout << "	R3BKeepEventProbabilityMfi::SetProbability " << fiber << " " << prob << endl;
}

Double_t R3BKeepEventProbabilityMfi::GetProbability(Int_t fiber)
{
    // for debugging:
    if (fiber > 1025 || fiber < 0)
    {
        // 		cout << "	R3BKeepEventProbabilityMfi::GetProbability Invalid fiber number " << fiber << " Skipping
        // event!
        // "
        // << endl;
        return 0.0;
    }

    // 	cout << "	R3BKeepEventProbabilityMfi::GetProbability(" << fiber << ") " << fKeepEventProb[fiber] << endl;
    return fKeepEventProb[fiber];
}

void R3BKeepEventProbabilityMfi::ReadProbabilityFromFile(string filename)
{
    ifstream infile;
    string line;

    infile.open(filename.c_str());

    // 	Int_t counter=0;
    Int_t fiber;
    Double_t prob;

    size_t pos; // position of delimiter

    // Read line by line
    // parse for probabilities
    while (infile.good())
    {
        // 		counter ++;
        // 		if(counter>400){break;}

        getline(infile, line);

        // ignore comments
        if ((line[0] == '/') && (line[1] == '/'))
        {
            continue;
        }

        // take substring after '='
        // delete everything before '=' (stemming from ggland input)
        pos = line.find_first_of('=');
        line = line.substr(pos + 1);

        // delete ',' at the end of each line
        pos = line.find_first_of(',');
        line = line.substr(0, pos);

        // get fiber number before ':' and probability after it
        // fiber number starts counting from 1
        pos = line.find_first_of(':');
        fiber = atoi((line.substr(0, pos)).c_str());
        prob = Double_t(atof((line.substr(pos + 1)).c_str()));

        // 		cout << line << " extracted: " << fiber << " " << prob << endl;

        // save it to array
        // 		this->SetProbability(fiber, prob);
        SetProbability(fiber, prob);
    }
}
