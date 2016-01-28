#ifndef R3BLANDTSYNCMODULEPAR_H
#define R3BLANDTSYNCMODULEPAR_H

#include "FairParGenericSet.h"

#define NCHMAX 5000

class FairParamList;


class R3BLandTSyncModulePar : public FairParGenericSet
{
public:
    /**
     * Standard constructor.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BLandTSyncModulePar ( const char* name = "TSyncModulePar",
                           const char* title = "Time synchronization of a module",
                           const char* context = "TestDefaultContext",
                           Bool_t own = kTRUE );

    /**
     * Destructor.
     * Frees the memory allocated by the object.
     */
    virtual ~R3BLandTSyncModulePar ( void );

    /**
     * A method to reset the parameter values. Sets all parameters to 0.
     */
    void clear ( void );

    /**
     * A method to write parameters using RuntimeDB.
     * @param list a list of parameters.
     */
    void putParams ( FairParamList* list );

    /**
     * A method to read parameters using RuntimeDB.
     * @param list a list of parameters.
     * @return kTRUE if successfull, else kFALSE.
     */
    Bool_t getParams ( FairParamList* list );

    /**
     * A method to print value of parameters to the standard
     * output using FairLogger.
     */
    void printParams();

    /** Accessor functions **/
    Int_t GetModuleId() const {
        return fModuleId;
    }
    Int_t GetSide() const {
        return fSide;
    }
    Double_t GetTimeOffset() const {
        return fTimeOffset;
    }
    Double_t GetEnergieGain() const {
        return fEnergieGain;
    }
    Double_t GetEffectiveSpeed() const {
	return fEffectiveSpeed;
    }
    void SetModuleId ( Int_t i ) {
        fModuleId = i;
    }
    void SetSide ( Int_t i ) {
        fSide = i;
    }
    void SetTimeOffset ( Double_t i ) {
        fTimeOffset = i;
    }
    void SetEnergieGain ( Double_t i ) {
        fEnergieGain = i;
    }
    void SetEffectiveSpeed ( Double_t i ){
	fEffectiveSpeed = i;
    }

private:
    Int_t fModuleId;          /**< Index of a detector module. */
    Int_t fSide;              /**< Side of a module: for NeuLAND - L/R PMT. */
    Double_t fTimeOffset;
    Double_t fEnergieGain;
    Double_t fEffectiveSpeed;
    ClassDef ( R3BLandTSyncModulePar, 1 );
};

#endif /* !R3BLANDTSYNCMODULEPAR_H*/
