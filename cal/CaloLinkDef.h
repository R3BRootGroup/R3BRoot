#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
 
#pragma link C++ class R3BGeoCalo+;
#pragma link C++ class R3BCaloGeometry+;
#pragma link C++ class R3BCaloHitFinder+;
#pragma link C++ class R3BCaloHitFinderPar+;
#pragma link C++ class R3BGeoCaloPar;
#pragma link C++ class R3BCalo+;
#pragma link C++ class R3BCaloContFact;

#pragma link C++ class R3BCaloUnpack+;
#pragma link C++ class R3BCaloUnpackPar+;
#pragma link C++ class R3BCaloRawAna+;
#ifdef WITH_FAIRDB
#pragma link C++ class R3BCaloCal+;
#endif
#pragma link C++ class R3BCaloCrystalAna+;
#pragma link C++ class R3BCaloCalibParFinder+;
#pragma link C++ class R3BCaloMap+;
#pragma link C++ class R3BCaloPIDCal+;


#endif

