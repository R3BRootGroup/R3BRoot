#ifndef R3BTOFDHITMODULEPAR_H
#define R3BTOFDHITMODULEPAR_H

#include "FairParGenericSet.h"

#define NCHMAX 5000

class FairParamList;

/**
 * Container for time calibration of a single module. This class is used for
 * storage of Tofd hit parameter
 */

class R3BTofdHitModulePar : public FairParGenericSet
{
  public:
    /**
     * Standard constructor.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BTofdHitModulePar(const char* name = "TofdHitModulePar",
                        const char* title = "TOFd hit calibration of a module",
                        const char* context = "TestDefaultContext",
                        Bool_t own = kTRUE);

    /**
     * Destructor.
     * Frees the memory allocated by the object.
     */
    virtual ~R3BTofdHitModulePar(void);

    /**
     * A method to reset the parameter values. Sets all parameters to 0.
     */
    void clear(void);

    /**
     * A method to write parameters using RuntimeDB.
     * @param list a list of parameters.
     */
    void putParams(FairParamList* list);

    /**
     * A method to read parameters using RuntimeDB.
     * @param list a list of parameters.
     * @return kTRUE if successfull, else kFALSE.
     */
    Bool_t getParams(FairParamList* list);

    /**
     * A method to print value of parameters to the standard
     * output using FairLogger.
     */
    void printParams();

    /**
     * A method to draw vaues of parameters on the current Canvas.
     */
    void DrawParams();

    /** Accessor functions **/
    Int_t GetPlane() const { return fPlane; }
    Int_t GetPaddle() const { return fPaddle; }
    Double_t GetSync() const { return fSync; }
    Double_t GetOffset1() const { return fOffset1; }
    Double_t GetOffset2() const { return fOffset2; }
    Double_t GetVeff() const { return fVeff; }
    Double_t GetPar1a() const { return fPar1a; }
    Double_t GetPar1b() const { return fPar1b; }
    Double_t GetPar1c() const { return fPar1c; }
    Double_t GetPar1d() const { return fPar1d; }
    Double_t GetPar2a() const { return fPar2a; }
    Double_t GetPar2b() const { return fPar2b; }
    Double_t GetPar2c() const { return fPar2c; }
    Double_t GetPar2d() const { return fPar2d; }
    Double_t GetPar1za() const { return fPar1za; }
    Double_t GetPar1zb() const { return fPar1zb; }
    Double_t GetPar1zc() const { return fPar1zc; }
    Double_t GetPar1walk() const { return fPar1walk; }
    Double_t GetPar2walk() const { return fPar2walk; }
    Double_t GetPar3walk() const { return fPar3walk; }
    Double_t GetPar4walk() const { return fPar4walk; }
    Double_t GetPar5walk() const { return fPar5walk; }
    void SetPlane(Int_t i) { fPlane = i; }
    void SetPaddle(Int_t i) { fPaddle = i; }
    void SetSync(Double_t t) { fSync = t; }
    void SetOffset1(Double_t t) { fOffset1 = t; }
    void SetOffset2(Double_t t) { fOffset2 = t; }
    void SetPar1a(Double_t par1a) { fPar1a = par1a; }
    void SetPar1b(Double_t par1b) { fPar1b = par1b; }
    void SetPar1c(Double_t par1c) { fPar1c = par1c; }
    void SetPar1d(Double_t par1d) { fPar1d = par1d; }
    void SetPar2a(Double_t par2a) { fPar2a = par2a; }
    void SetPar2b(Double_t par2b) { fPar2b = par2b; }
    void SetPar2c(Double_t par2c) { fPar2c = par2c; }
    void SetPar2d(Double_t par2d) { fPar2d = par2d; }
    void SetPar1za(Double_t par1za) { fPar1za = par1za; }
    void SetPar1zb(Double_t par1zb) { fPar1zb = par1zb; }
    void SetPar1zc(Double_t par1zc) { fPar1zc = par1zc; }
    void SetPar1walk(Double_t par1walk) { fPar1walk = par1walk; }
    void SetPar2walk(Double_t par2walk) { fPar2walk = par2walk; }
    void SetPar3walk(Double_t par3walk) { fPar3walk = par3walk; }
    void SetPar4walk(Double_t par4walk) { fPar4walk = par4walk; }
    void SetPar5walk(Double_t par5walk) { fPar5walk = par5walk; }
    void SetVeff(Double_t v) { fVeff = v; }

  private:
    Int_t fPlane;      /**< Index of a plane. */
    Int_t fPaddle;     /**< Index of a paddle. */
    Double_t fOffset1; /**< time offset of PM1 */
    Double_t fOffset2; /**< time offset of PM2 */
    Double_t fVeff;    /**< effective velocity of light in paddle for position determination */
    Double_t fSync;    /**< time offset between paddles */
    Double_t fPar1a, fPar1b, fPar1c, fPar1d;
    Double_t fPar2a, fPar2b, fPar2c, fPar2d;
    Double_t fPar1za, fPar1zb, fPar1zc, fPar1zd;
    Double_t fPar1walk, fPar2walk, fPar3walk, fPar4walk, fPar5walk;

    ClassDef(R3BTofdHitModulePar, 1);
};

#endif
