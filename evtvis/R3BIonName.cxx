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

#include <array>
#include <string_view>

#include "R3BIonName.h"

// -----   Default constructor   -------------------------------------------
R3BIonName::R3BIonName()
    : TNamed()
{
}

Bool_t R3BIonName::GetIonName(Int_t element, Char_t* name)
{
    static constexpr std::array<std::string_view, 117> ionNames = {
        "", // index 0 (unused)
        "", // index 1 (unused)
        "He", "Li", "Be", "B",  "C",  "N",  "O",  "F",   "Ne",  "Na",  "Mg",  "Al",  "Si", "P",  "S",  "Cl", "Ar",
        "K",  "Ca", "Sc", "Ti", "V",  "Cr", "Mn", "Fe",  "Co",  "Ni",  "Cu",  "Zn",  "Ga", "Ge", "As", "Se", "Br",
        "Kr", "Rb", "Sr", "Y",  "Zr", "Nb", "Mo", "Tc",  "Ru",  "Rh",  "Pd",  "Ag",  "Cd", "In", "Sn", "Sb", "Te",
        "I",  "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",  "Pm",  "Sm",  "Eu",  "Gd",  "Tb", "Dy", "Ho", "Er", "Tm",
        "Yb", "Lu", "Hf", "Ta", "W",  "Re", "Os", "Ir",  "Pt",  "Au",  "Hg",  "Tl",  "Pb", "Bi", "Po", "At", "Rn",
        "Fr", "Ra", "Ac", "Th", "Pa", "U",  "Np", "Pu",  "Am",  "Cm",  "Bk",  "Cf",  "Es", "Fm", "Md", "No", "Lr",
        "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Uuu", "Uub", "Uut", "Uuq", "Uup", "Uuh"
    };

    if (element < 2 || element >= static_cast<int>(ionNames.size()) || ionNames[element].empty())
        return false;

    std::string_view symbol = ionNames[element];
    std::snprintf(name, symbol.size() + 1, "%s", symbol.data());
    return true;
}

ClassImp(R3BIonName)
