#ifndef R3BEVENTMANAGEREDITOR
#define R3BEVENTMANAGEREDITOR

#include "FairEventManagerEditor.h"

class R3BEventManagerEditor : public FairEventManagerEditor
{
protected:
TGCheckButton  *fScaleE;

public:

R3BEventManagerEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30,
                      UInt_t options = kChildFrame, 
		      Pixel_t back=GetDefaultFrameBackground());
virtual ~R3BEventManagerEditor() {}
 

 virtual void Init();
 virtual void SelectPDG(); 
virtual void SelectEvent(); 
 virtual void MaxEnergy();
 virtual void DoScaleE();

 ClassDef(R3BEventManagerEditor, 0); 
};

#endif
