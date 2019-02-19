/* R3BChannelAccessEPICS.h
 * R3BROOT
 *
 * Author: Hans Törnqvist <h.toernqvist@gsi.de>
 * Date: 06/12/17
 * */

/*
 * Note that you should probably set some EPICS environment variables
 * properly before going to town, if not to at all get access IOC:s but also
 * to not cause havoc on the network, e.g.:
 *
 *  export EPICS_CA_AUTO_ADDR_LIST=no
 *  export EPICS_CA_ADDR_LIST=123.45.67.89
 *  root -l macro_mania.C
 */

#ifndef __R3BROOT__R3BCHANNELACCESSEPICS__
#define __R3BROOT__R3BCHANNELACCESSEPICS__

#include "R3BChannelAccess.h"
#include <cadef.h>

class R3BChannelAccessEPICS : public R3BChannelAccess
{
  public:
    R3BChannelAccessEPICS(TString const& = "");
    ~R3BChannelAccessEPICS();
    double Get();
    double* GetPointer();
    chid GetID() const;
    chtype GetType() const;
    void Set(double);

    ClassDef(R3BChannelAccessEPICS, 1)

        private : TString fName;
    double fValue;
    chid fId;
    chtype fType;
};

class R3BChannelAccessGroupEPICS : public R3BChannelAccessGroup
{
  public:
    R3BChannelAccessGroupEPICS();
    ~R3BChannelAccessGroupEPICS();
    R3BChannelAccess* CreateChannel(TString const&);
    bool Commit();
    bool Fetch();

    ClassDef(R3BChannelAccessGroupEPICS, 1)

        private : bool PendIO(char const*);
    bool Search();

    TClonesArray fChannelArray;
    bool fNeedSearch;
};

class R3BChannelAccessMasterEPICS : public R3BChannelAccessMaster
{
  public:
    R3BChannelAccessMasterEPICS();
    ~R3BChannelAccessMasterEPICS();
    R3BChannelAccessGroup* CreateGroup();

    ClassDef(R3BChannelAccessMasterEPICS, 1)

        private : TClonesArray fGroupArray;
};

#endif
