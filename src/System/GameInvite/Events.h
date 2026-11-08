// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once
#include "Bridge.h"
#include <xgameinvite.h>
#include <xgameactivation.h>
#include <xgameprotocol.h>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace nexus::invites
{
enum class EventKind { Accepted, Pending, Activation, Protocol };
struct EventCallbacks
{
    XGameInviteEventCallback *invite = nullptr;
    XGameActivationCallback *activation = nullptr;
    XGameProtocolActivationCallback *protocol = nullptr;
};
class Events
{
    struct Registration;
    struct Delivery;
    std::mutex lock;
    std::condition_variable changed;
    std::map<UINT64, std::shared_ptr<Registration>> registrations;
    UINT64 serial = 0;
    std::string accepted, pending, cursor = "0";
    bool worker = false, useWorker;
    Transport &transport;
    static DWORD WINAPI Run( void *context );
    static void CALLBACK Callback( void *context, BOOLEAN canceled );
    HRESULT Submit( const std::shared_ptr<Registration> &registration, const std::string &uri, bool isPending );
public:
    explicit Events( Transport &transport, bool useWorker = true ) : useWorker( useWorker ), transport( transport ) {}
    HRESULT Register( EventKind kind, XTaskQueueHandle queue, void *context, EventCallbacks callbacks, XTaskQueueRegistrationToken *token );
    BOOLEAN Unregister( EventKind kind, XTaskQueueRegistrationToken token, BOOLEAN wait );
    void Deliver( const std::string &uri, bool isPending );
    void Revoke();
};
Events &GetEvents() noexcept;
HRESULT Accept( const char *uri ) noexcept;
HRESULT ShowInvite( XAsyncBlock *async, XUserHandle user, const char *scid, const char *name, const char *session, const char *text, const char *context );
HRESULT ShowInviteResult( XAsyncBlock *async );
HRESULT ShowAchievements( XAsyncBlock *async, XUserHandle user, UINT32 title );
HRESULT ShowAchievementsResult( XAsyncBlock *async );
HRESULT ShowActivityInvite( XAsyncBlock *async, XUserHandle user );
HRESULT ShowActivityInviteResult( XAsyncBlock *async );
}
