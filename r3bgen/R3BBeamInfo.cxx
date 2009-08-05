
#include "R3BBeamInfo.h"

ClassImp(R3BBeamInfo)
  
R3BBeamInfo::R3BBeamInfo() {
  //
  // Init values
  //
  theta = 0;        
  phi =0;          

  xPos = 0.;
  yPos = 0.;
  zPos = 0.;
}


R3BBeamInfo::~R3BBeamInfo(){
  //
  // Destructor. Makes nothing
  //
}


void R3BBeamInfo::print(void){
  //
  // Prints info
  //
}


void R3BBeamInfo::SetVertexPosition(Double_t x, Double_t y, Double_t z) {
  //
  // Sets the position of the vertex (origin of the primary particle)
  //
  xPos = x;
  yPos = y;
  zPos = z;
}


void R3BBeamInfo::SetAngles(Double_t angleTheta, Double_t anglePhi){
  //
  // Sets the 4momentum
  //
  theta = angleTheta;
  phi = anglePhi;
}
