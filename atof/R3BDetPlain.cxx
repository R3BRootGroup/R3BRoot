#include "R3BDetPlain.h"

/********************************************************************/
R3BDetPlain::R3BDetPlain(){
    SetName("R3BDetPlain");
    Id = 0;
}

/********************************************************************/
R3BDetPlain::R3BDetPlain(std::string Name){
    SetName(Name.c_str());
    Id = 0;
}

/********************************************************************/
R3BDetPlain::~R3BDetPlain(){
}

/********************************************************************/
void R3BDetPlain::Display(){
    std::cout << "Welcome to the R3BDetPlain class!!!\n";
    std::cout << "Detector Name: " << GetName() << "\n";
    std::cout << "Plain Z position: " << Z << "\n";
    std::cout << "Plain X or Y position: " << XorY << "\n";
    std::cout << "TiltAngle : " << TiltAngle << "\n";
    std::cout << "Data:\n";
    for(UInt_t i=0;i<Pos.size();i++)
	std::cout << " Id: " << i << "; Pos: " << Pos[i] << ";\n";
}

/********************************************************************/
Double_t R3BDetPlain::GetPos(Int_t id){
    if(id < 0 || id >= (Int_t) Pos.size()) return 0;
    return Pos[id];
}

/********************************************************************/
void R3BDetPlain::AddRaw(Int_t id,Double_t pos){
    if(id != Id){
	std::cout << "!> Fatal Error!!! The wrong Id of the Raw ";
	std::cout << id << " != " << Id << "\n";
	std::exit(1);
    }
    Id++;
    Pos.push_back(pos);
}

/********************************************************************/
ClassImp(R3BDetPlain)
