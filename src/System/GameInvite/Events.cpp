// SPDX-License-Identifier: LGPL-2.1-or-later
#include "Events.h"
#include "Network.h"
#include "../../Runtime/Configuration.h"
#include <new>

namespace nexus::invites
{
struct EventWorker { Events *events; HMODULE module; };
struct Events::Registration
{
    Events *owner;
    EventKind kind;
    EventCallbacks callbacks;
    IXThreadingImpl *threading = nullptr;
    XTaskQueueHandle queue = nullptr;
    void *context;
    bool removed = false;
    unsigned running = 0;
    ~Registration() { if ( threading ) { if ( queue ) threading->XTaskQueueCloseHandle( queue ); threading->Release(); } }
};
struct Events::Delivery { std::shared_ptr<Registration> registration; std::string uri; bool pending; };
static thread_local const void *currentRegistration;
void CALLBACK Events::Callback( void *context, BOOLEAN canceled )
{
    std::unique_ptr<Delivery> delivery( static_cast<Delivery *>( context ) );
    auto registration = delivery->registration;
    auto &owner = *registration->owner;
    std::unique_lock guard( owner.lock );
    if ( canceled || registration->removed ) return;
    ++registration->running;
    guard.unlock();
    struct Completion
    {
        Events &owner; Registration &registration; const void *previous;
        ~Completion() { currentRegistration = previous; std::lock_guard guard( owner.lock ); --registration.running; owner.changed.notify_all(); }
    } completion{ owner, *registration, currentRegistration };
    currentRegistration = registration.get();
    if ( registration->kind == EventKind::Activation )
    {
        XGameActivationInfo info{};
        info.type = delivery->pending ? XGameActivationType::PendingGameInvite : XGameActivationType::AcceptedGameInvite;
        info.inviteUri = delivery->uri.c_str();
        registration->callbacks.activation( registration->context, &info );
    }
    else if ( registration->kind == EventKind::Protocol ) registration->callbacks.protocol( registration->context, delivery->uri.c_str() );
    else registration->callbacks.invite( registration->context, delivery->uri.c_str() );
}
HRESULT Events::Submit( const std::shared_ptr<Registration> &registration, const std::string &uri, bool isPending )
{
    auto *delivery = new ( std::nothrow ) Delivery{ registration, uri, isPending };
    if ( !delivery ) return E_OUTOFMEMORY;
    const auto hr = registration->threading->XTaskQueueSubmitCallback( registration->queue, XTaskQueuePort::Completion, delivery, Callback );
    if ( FAILED( hr ) ) delete delivery;
    return hr;
}
HRESULT Events::Register( EventKind kind, XTaskQueueHandle queue, void *context, EventCallbacks callbacks, XTaskQueueRegistrationToken *token ) try
{
    if ( token ) token->token = 0;
    if ( !token || ( kind == EventKind::Activation ? !callbacks.activation : kind == EventKind::Protocol ? !callbacks.protocol : !callbacks.invite ) ) return E_INVALIDARG;
    if ( useWorker && !BridgeAvailable() ) return E_NOTIMPL;
    auto registration = std::make_shared<Registration>();
    registration->owner = this; registration->kind = kind; registration->callbacks = callbacks; registration->context = context;
    auto hr = QueryApiImpl( CLSID_XThreadingImpl, IID_IXThreadingImpl, reinterpret_cast<void **>( &registration->threading ) );
    if ( FAILED( hr ) ) return hr;
    hr = queue ? registration->threading->XTaskQueueDuplicateHandle( queue, &registration->queue )
               : registration->threading->XTaskQueueGetCurrentProcessTaskQueue( &registration->queue ) ? S_OK : E_FAIL;
    if ( FAILED( hr ) ) return hr;
    std::unique_lock guard( lock );
    if ( serial == UINT64_MAX ) return E_OUTOFMEMORY;
    const auto initial = kind == EventKind::Pending ? pending : kind == EventKind::Activation && !pending.empty() ? pending : accepted;
    const bool isPending = kind == EventKind::Pending || ( kind == EventKind::Activation && !pending.empty() );
    registrations.emplace( ++serial, registration ); token->token = serial;
    if ( useWorker && !worker )
    {
        HMODULE module = nullptr;
        if ( !GetModuleHandleExW( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<const WCHAR *>( &Run ), &module ) ) hr = HRESULT_FROM_WIN32( GetLastError() );
        else
        {
            auto *start = new ( std::nothrow ) EventWorker{ this, module };
            const auto thread = start ? CreateThread( nullptr, 0, Run, start, 0, nullptr ) : nullptr;
            if ( !thread ) { delete start; FreeLibrary( module ); hr = E_OUTOFMEMORY; }
            else { worker = true; CloseHandle( thread ); }
        }
    }
    if ( FAILED( hr ) ) { registrations.erase( token->token ); token->token = 0; }
    guard.unlock();
    if ( SUCCEEDED( hr ) && !initial.empty() ) hr = Submit( registration, initial, isPending );
    if ( FAILED( hr ) && token->token ) { Unregister( kind, *token, FALSE ); token->token = 0; }
    return hr;
}
catch ( ... ) { if ( token && token->token ) Unregister( kind, *token, FALSE ); if ( token ) token->token = 0; return E_OUTOFMEMORY; }
BOOLEAN Events::Unregister( EventKind kind, XTaskQueueRegistrationToken token, BOOLEAN wait )
{
    std::unique_lock guard( lock );
    const auto found = registrations.find( token.token );
    if ( found == registrations.end() || found->second->kind != kind ) return FALSE;
    auto registration = found->second; registration->removed = true; registrations.erase( found );
    while ( wait && registration->running && currentRegistration != registration.get() ) changed.wait( guard );
    const auto done = registration->running == 0;
    guard.unlock(); // Queue cleanup must not run while holding the registry lock.
    return done;
}
void Events::Deliver( const std::string &uri, bool isPending ) try
{
    if ( uri.empty() || uri.size() > 4096 ) return;
    std::vector<std::shared_ptr<Registration>> selected;
    std::unique_lock guard( lock );
    if ( isPending ) { if ( pending == uri ) return; pending = uri; }
    else { accepted = uri; if ( pending == uri ) pending.clear(); }
    for ( const auto &[id, registration] : registrations )
        if ( registration->kind == EventKind::Activation || ( isPending ? registration->kind == EventKind::Pending : registration->kind != EventKind::Pending ) ) selected.push_back( registration );
    guard.unlock();
    for ( const auto &registration : selected ) Submit( registration, uri, isPending );
}
catch ( ... ) {}
void Events::Revoke()
{
    std::map<UINT64, std::shared_ptr<Registration>> removed;
    std::unique_lock guard( lock );
    for ( const auto &[id, registration] : registrations ) registration->removed = true;
    removed.swap( registrations ); accepted.clear(); pending.clear();
    guard.unlock();
}
DWORD WINAPI Events::Run( void *context )
{
    const auto start = *static_cast<EventWorker *>( context ); delete static_cast<EventWorker *>( context );
    auto &events = *start.events;
    const auto com = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    try
    {
    Notifications notifications;
    for ( ;; )
    {
        bool active;
        { std::lock_guard guard( events.lock ); active = !events.registrations.empty(); if ( !active ) events.worker = false; }
        if ( !active ) break;
        std::vector<std::string> reply;
        const auto *config = nexus::GetGameConfiguration();
        XUserHandle user = nullptr;
        if ( config && SUCCEEDED( x_user_impl->XUserFindUserByLocalId( XUserLocalId{ 1 }, &user ) ) )
        {
            const auto hr = events.transport.Exchange( { "hello-events", std::to_string( xuser_get_xuid( user ) ), std::to_string( config->titleId ) }, reply );
            if ( SUCCEEDED( hr ) )
            {
                notifications.Tick( user, config->titleId, [&]( const std::string &id, const std::string &uri, const std::string &sender, UINT64 expires ) {
                    if ( SUCCEEDED( events.transport.Exchange( { "notify", id, uri, sender, std::to_string( expires ) }, reply ) ) ) events.Deliver( uri, true );
                }, [&]( const AchievementUnlock &unlock ) {
                    GetAchievementWatcher().Report( unlock, [&]( const AchievementUnlock &item ) { ForwardAchievement( events.transport, item ); } );
                } );
                reply.clear();
                if ( SUCCEEDED( events.transport.Exchange( { "events", events.cursor }, reply ) ) && reply.size() >= 3 )
                {
                    events.cursor = reply[0];
                    if ( reply.size() == 3 && reply[1] == "accepted" ) events.Deliver( reply[2], false );
                    else if ( reply.size() == 4 && reply[1] == "activity" )
                    {
                        const auto id = reply[2], recipient = reply[3];
                        const auto result = SendActivityInvite( GetNetwork(), user, xuser_get_xuid( user ), config->titleId, recipient );
                        events.transport.Exchange( { "complete", id, std::to_string( static_cast<UINT32>( result ) ) }, reply );
                    }
                }
            }
            else { notifications.Close(); if ( hr == E_ACCESSDENIED ) events.Revoke(); }
            x_user_impl->XUserCloseHandle( user );
        }
        Sleep( 500 );
    }
    }
    catch ( ... ) { std::lock_guard guard( events.lock ); events.worker = false; }
    if ( SUCCEEDED( com ) ) CoUninitialize();
    FreeLibraryAndExitThread( start.module, 0 );
}
Events &GetEvents() noexcept { static Events events( GetBridge() ); return events; }
HRESULT Accept( const char *uri ) noexcept
{
    if ( !uri || !*uri || strlen( uri ) > 4096 ) return E_INVALIDARG;
    std::vector<std::string> reply; return GetBridge().Exchange( { "accept", uri }, reply );
}
}
