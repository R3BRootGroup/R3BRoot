// -------------------------------------------------------------------------
// -----                      R3BGfiPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BGfiDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;



// -----   Default constructor   -------------------------------------------
R3BGfiDigi::R3BGfiDigi()
  : Gf1mul(0), Gf1_01x(0.)//,
    //Gf2mul(0), Gf2_01x(0.)   
{
}



R3BGfiDigi::R3BGfiDigi(Int_t gf1mul, Double_t gf1_01x)
  : Gf1mul(gf1mul), Gf1_01x(gf1_01x)//,
    //Gf2mul(gf2mul), Gf2_01x(gf2_01x)
{
}



R3BGfiDigi::R3BGfiDigi(const R3BGfiDigi& right)
  : Gf1mul(right.Gf1mul), Gf1_01x(right.Gf1_01x)
{
}



// -----   Destructor   ----------------------------------------------------
R3BGfiDigi::~R3BGfiDigi()
{
}



// -----   Public method Print   -------------------------------------------
void R3BGfiDigi::Print(const Option_t* opt) const
{
}
// -------------------------------------------------------------------------



ClassImp(R3BGfiDigi)
