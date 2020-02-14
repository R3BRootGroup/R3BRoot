// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "ObjDouble.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_ObjDouble(void *p = 0);
   static void *newArray_ObjDouble(Long_t size, void *p);
   static void delete_ObjDouble(void *p);
   static void deleteArray_ObjDouble(void *p);
   static void destruct_ObjDouble(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ObjDouble*)
   {
      ::ObjDouble *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ObjDouble >(0);
      static ::ROOT::TGenericClassInfo 
         instance("ObjDouble", ::ObjDouble::Class_Version(), "ObjDouble.h", 18,
                  typeid(::ObjDouble), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ObjDouble::Dictionary, isa_proxy, 4,
                  sizeof(::ObjDouble) );
      instance.SetNew(&new_ObjDouble);
      instance.SetNewArray(&newArray_ObjDouble);
      instance.SetDelete(&delete_ObjDouble);
      instance.SetDeleteArray(&deleteArray_ObjDouble);
      instance.SetDestructor(&destruct_ObjDouble);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ObjDouble*)
   {
      return GenerateInitInstanceLocal((::ObjDouble*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ObjDouble*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr ObjDouble::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *ObjDouble::Class_Name()
{
   return "ObjDouble";
}

//______________________________________________________________________________
const char *ObjDouble::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ObjDouble*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int ObjDouble::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ObjDouble*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ObjDouble::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ObjDouble*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ObjDouble::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ObjDouble*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void ObjDouble::Streamer(TBuffer &R__b)
{
   // Stream an object of class ObjDouble.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(ObjDouble::Class(),this);
   } else {
      R__b.WriteClassBuffer(ObjDouble::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_ObjDouble(void *p) {
      return  p ? new(p) ::ObjDouble : new ::ObjDouble;
   }
   static void *newArray_ObjDouble(Long_t nElements, void *p) {
      return p ? new(p) ::ObjDouble[nElements] : new ::ObjDouble[nElements];
   }
   // Wrapper around operator delete
   static void delete_ObjDouble(void *p) {
      delete ((::ObjDouble*)p);
   }
   static void deleteArray_ObjDouble(void *p) {
      delete [] ((::ObjDouble*)p);
   }
   static void destruct_ObjDouble(void *p) {
      typedef ::ObjDouble current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::ObjDouble

namespace {
  void TriggerDictionaryInitialization_dict_Impl() {
    static const char* headers[] = {
"ObjDouble.h",
0
    };
    static const char* includePaths[] = {
"/home/christiaan/Desktop/FAIR/FairSoft_Build/include/root",
"/home/christiaan/Desktop/FAIR/R3BRoot_Source/R3BRoot/DNN/Experiment/SAMURAI_7LiPN/FOLD/MDA/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$ObjDouble.h")))  ObjDouble;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "ObjDouble.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"ObjDouble", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_dict() {
  TriggerDictionaryInitialization_dict_Impl();
}
