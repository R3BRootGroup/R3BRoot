
#ifndef R3B_TRACKING_SETUP
#define R3B_TRACKING_SETUP

#include <vector>
#include <map>
#include <string>
#include "R3BTrackingDetector.h"

class R3BTrackingSetup
{
  public:
    R3BTrackingSetup();
    ~R3BTrackingSetup();

    void AddDetector(const std::string& name,
                     EDetectorType type,
                     const std::string& geoParName,
                     const std::string& dataName = "");

    R3BTrackingDetector* GetByName(const std::string& name);
    
    R3BTrackingDetector* GetFirstByType(const EDetectorType& type);

    void Init();

    void SetParContainers();

    void CopyHits();

    std::vector<R3BTrackingDetector*>& GetArray() { return fDetectors; }
    
    Double_t GetAfterGladResolution();

  private:
    std::vector<R3BTrackingDetector*> fDetectors;
    std::map<std::string, int> fMapIndex;

    ClassDef(R3BTrackingSetup, 1)
};

#endif
