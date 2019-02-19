/* R3BChannelAccess.h
 * R3BROOT
 *
 * Author: Hans Törnqvist <h.toernqvist@gsi.de>
 * Date: 05/12/17
 * */

#ifndef __R3BROOT__R3BCHANNELACCESS__
#define __R3BROOT__R3BCHANNELACCESS__

#include <TClonesArray.h>
#include <TString.h>

// Local storage for one channel.
class R3BChannelAccess : public TObject
{
  public:
    virtual ~R3BChannelAccess() {}

    // Returns local value.
    virtual double Get() = 0;

    // Sets local value.
    virtual void Set(double) = 0;
};

// One group of channels.
class R3BChannelAccessGroup : public TObject
{
  public:
    virtual ~R3BChannelAccessGroup() {}

    // Creates a channel.
    virtual R3BChannelAccess* CreateChannel(TString const&) = 0;

    // Writes all channels to remote.
    virtual bool Commit() = 0;

    // Reads all channels from remote.
    virtual bool Fetch() = 0;
};

// Top level resource handler.
class R3BChannelAccessMaster : public TObject
{
  public:
    virtual ~R3BChannelAccessMaster() {}

    // Creates a group.
    virtual R3BChannelAccessGroup* CreateGroup() = 0;
};

#endif
