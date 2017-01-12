#ifndef TCACONNECTOR
#define TCACONNECTOR

#include <exception>
#include <vector>

#include "TClonesArray.h"
#include "TString.h"

#include "FairRootManager.h"

template <typename T>
class TCAInputConnector
{
  private:
    TString fBranchName;
    TString fClassName;
    TClonesArray* fTCA; // non-owning

  public:
    TCAInputConnector(const TString& b)
        : fBranchName(b)
        , fClassName(T().ClassName())
    {
    }

    void Init()
    {
        FairRootManager* ioman = FairRootManager::Instance();
        if (ioman == nullptr)
        {
            throw std::runtime_error("TCAInputConnector: No FairRootManager");
        }
        fTCA = (TClonesArray*)ioman->GetObject(fBranchName);
        if (fTCA == nullptr)
        {
            throw std::runtime_error(TString("TCAInputConnector: No TClonesArray called " + fBranchName +
                                             " could be obtained from the FairRootManager")
                                         .Data());
        }
        if (!TString(fTCA->GetClass()->GetName()).EqualTo(fClassName))
        {
            throw std::runtime_error(TString("TCAInputConnector: TClonesArray " + fBranchName +
                                             " does not contain elements of type " + fClassName)
                                         .Data());
        }
    }

    std::vector<T*> Retrieve() const
    {
        if (fTCA == nullptr)
        {
            throw std::runtime_error("TCAInputConnector: TClonesArray not available");
        }

        std::vector<T*> fV;

        const Int_t n = fTCA->GetEntries();
        fV.reserve(n);
        for (Int_t i = 0; i < n; i++)
        {
            fV.push_back((T*)fTCA->At(i));
        }
        return fV;
    }

    std::vector<T> RetrieveObjects() const
    {
        if (fTCA == nullptr)
        {
            throw std::runtime_error("TCAInputConnector: TClonesArray not available");
        }

        std::vector<T> fV;

        const Int_t n = fTCA->GetEntries();
        fV.reserve(n);
        for (Int_t i = 0; i < n; i++)
        {
            fV.push_back(*(T*)fTCA->At(i));
        }
        return fV;
    }
};

template <typename T>
class TCAOutputConnector
{
  private:
    TString fBranchName;
    TString fClassName;
    TClonesArray* fTCA; // non-owning

  public:
    TCAOutputConnector(const TString& b)
        : fBranchName(b)
        , fClassName(T().ClassName())
    {
    }

    void Init()
    {
        FairRootManager* ioman = FairRootManager::Instance();
        if (ioman == nullptr)
        {
            throw std::runtime_error("TCAOutputConnector: No FairRootManager");
        }
        fTCA = ioman->Register(fBranchName, fClassName, "", kTRUE);
    }

    void Store(std::vector<T>& v)
    {
        if (fTCA == nullptr)
        {
            throw std::runtime_error("TCAOutputConnector: TClonesArray not available");
        }

        fTCA->Clear();

        for (auto& o : v)
        {
            new ((*fTCA)[fTCA->GetEntries()]) T(std::move(o));
        }

        v.clear();
    }
};

#endif // TCACONNECTOR
