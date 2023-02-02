#include "R3BCalifaLegacyWriter.h"

#include <FairRootManager.h>
#include <TClonesArray.h>
#include "R3BLogger.h"

#include "R3BCalifaMappedData.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaClusterData.h"

namespace{
template<class K, class V>
const V& extractValue(const std::pair<K, V>& val)
{
  return val.second;
}

template<class V>
const V& extractValue(V& val)
{
  return val;
}

}
template<typename T>
void addConverter(std::vector<std::function<void(void)>>& funList)
{
  FairRootManager* frm = FairRootManager::Instance();
  const char *name=T::Class()->GetName();
  assert(frm);  
  auto stlptr=frm->InitObjectAs<const typename T::container_t*>(T::default_container_name);
  if (!stlptr)
    {
      LLOG(warning) << "Default container for "<<name <<" not found, will not create " << T::tca_name ;
      return;
    }
  auto& stl=*stlptr;
  auto& tca = *new TClonesArray(name);
  frm->Register(T::tca_name, name, &tca, 1);
  LLOG(info) << "Converting " << name << " from " << T::default_container_name << " to TClonesArray " << T::tca_name ;
  auto f=[&stl, &tca]()
	 {
	   tca.Clear();
	   for (auto& h: stl)
	     new (tca[tca.GetEntries()]) T(extractValue(h));
	   LLOG(debug) << "converted " << tca.GetEntries() << " entries to TClonesArray " << T::tca_name;
	 };
  funList.push_back(f);
}

InitStatus R3BCalifaLegacyWriter::Init()
{
  addConverter<R3BCalifaMappedData>(fOnExec);
  addConverter<R3BCalifaCrystalCalData>(fOnExec);
  addConverter<R3BCalifaClusterData>(fOnExec);
  LLOG(debug) << "Registered "<< fOnExec.size() << " converters.";
  return kSUCCESS;
}


void R3BCalifaLegacyWriter::Exec(const char*)
{
  for (auto& f: fOnExec)
    f();
}

ClassImp(R3BCalifaLegacyWriter);
