#ifndef R3BDETPLAIN_header
#define R3BDETPLAIN_header 1

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "TNamed.h"
#include <vector>

/********************************************************************/
class R3BDetPlain:public TNamed{
    private:
	std::vector<Double_t> Pos;
	Double_t Z,XorY;
	Int_t Id;
	Double_t TiltAngle;
    public:
	R3BDetPlain();
	R3BDetPlain(std::string Name);
	~R3BDetPlain();
	//
	void Display();
	//
	Double_t GetZ(){return Z;};
	void SetZ(Double_t z){Z = z;};
	//
	Double_t GetXorY(){return XorY;};
	void SetXorY(Double_t xory){XorY = xory;};
	//
	Double_t GetTiltAngle(){return TiltAngle;};
	void SetTiltAngle(Double_t tiltangle){TiltAngle = tiltangle;};
	//
	Double_t GetPos(Int_t id);
	void AddRaw(Int_t id,Double_t pos);
	//
	Int_t GetNRaws(){return (Int_t) Pos.size();};
	//
    /*------------------------------------------------------------*/
    ClassDef(R3BDetPlain,1); // R3BDetPlain
};

#endif
