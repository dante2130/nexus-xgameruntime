// SPDX-License-Identifier: LGPL-2.1-or-later
#include "Events.h"
#include "Network.h"
#include "Dialog.h"
#include "../Threading/AsyncIdentity.h"
#include "../../Runtime/Configuration.h"
#include <new>
namespace nexus::invites
{
static const char inviteIdentity = 0, activityIdentity = 0, achievementsIdentity = 0;
struct UiWork
{
    LONG refs = 1;
    std::atomic<bool> canceled{ false }, completed{ false };
    IXThreadingImpl *threading = nullptr;
    XAsyncBlock *async = nullptr;
    DialogTask task;
    void Retain() { InterlockedIncrement( &refs ); }
    void Release() { if ( !InterlockedDecrement( &refs ) ) delete this; }
    ~UiWork() { if ( threading ) threading->Release(); }
    void Complete( HRESULT hr ) { if ( !completed.exchange( true ) ) threading->XAsyncComplete( async, hr, 0 ); }
};
struct UiWorker { UiWork *work; HMODULE module; };
static DWORD WINAPI RunUi( void *context )
{
    const auto start = *static_cast<UiWorker *>( context ); delete static_cast<UiWorker *>( context );
    auto *work = start.work;
    HRESULT hr = E_ABORT;
    const auto com = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    try { if ( !work->canceled ) hr = work->task( work->canceled ); }
    catch ( ... ) { hr = E_OUTOFMEMORY; }
    if ( work->canceled ) hr = E_ABORT;
    work->Complete( hr ); work->Release();
    if ( SUCCEEDED( com ) ) CoUninitialize();
    FreeLibraryAndExitThread( start.module, 0 );
}
static HRESULT CALLBACK Provider( XAsyncOp operation, const XAsyncProviderData *data )
{
    auto *work = static_cast<UiWork *>( data->context );
    switch ( operation )
    {
    case XAsyncOp::Begin: return work->threading->XAsyncSchedule( data->async, 0 );
    case XAsyncOp::DoWork:
    {
        HMODULE module = nullptr;
        if ( !GetModuleHandleExW( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<const WCHAR *>( &RunUi ), &module ) ) return E_FAIL;
        auto *start = new ( std::nothrow ) UiWorker{ work, module };
        if ( !start ) { FreeLibrary( module ); return E_OUTOFMEMORY; }
        work->Retain(); const auto thread = CreateThread( nullptr, 0, RunUi, start, 0, nullptr );
        if ( !thread ) { work->Release(); delete start; FreeLibrary( module ); return E_OUTOFMEMORY; }
        CloseHandle( thread );
        return E_PENDING;
    }
    case XAsyncOp::Cancel: work->canceled = true; return S_OK;
    case XAsyncOp::GetResult: return S_OK;
    case XAsyncOp::Cleanup: work->Release(); return S_OK;
    default: return E_NOTIMPL;
    }
}
HRESULT ScheduleDialog( XAsyncBlock *async, bool activity, DialogTask task, bool achievements )
{
    if ( !async || !task ) return E_INVALIDARG;
    auto *work = new ( std::nothrow ) UiWork; if ( !work ) return E_OUTOFMEMORY;
    HRESULT hr = E_OUTOFMEMORY;
    try
    {
        work->task = std::move( task ); work->async = async;
        hr = QueryApiImpl( CLSID_XThreadingImpl, IID_IXThreadingImpl, reinterpret_cast<void **>( &work->threading ) );
        if ( SUCCEEDED( hr ) ) hr = work->threading->XAsyncBegin( async, work, achievements ? &achievementsIdentity : activity ? &activityIdentity : &inviteIdentity, achievements ? "XGameUiShowAchievementsAsync" : activity ? "XGameUiShowMultiplayerActivityGameInviteAsync" : "XGameUiShowSendGameInviteAsync", Provider );
    }
    catch ( ... ) { hr = E_OUTOFMEMORY; }
    if ( FAILED( hr ) ) work->Release();
    return hr;
}
struct InviteRequest
{
    XUserHandle user = nullptr;
    UINT32 title = 0;
    bool activity = false;
    std::string id;
    std::vector<std::string> session;
    ~InviteRequest() { if ( user ) x_user_impl->XUserCloseHandle( user ); }
    HRESULT Run( const std::atomic<bool> &canceled )
    {
        std::vector<std::string> reply;
        auto hr = GetBridge().Exchange( { "hello", std::to_string( xuser_get_xuid( user ) ), std::to_string( title ) }, reply );
        if ( SUCCEEDED( hr ) && !canceled )
        {
            auto request = session; request.insert( request.begin(), id ); request.insert( request.begin(), "present" );
            if ( activity ) request = { "present-activity", id };
            hr = GetBridge().Exchange( request, reply );
            const auto deadline = GetTickCount64() + 180000;
            if ( SUCCEEDED( hr ) ) for ( ;; )
            {
                if ( canceled || !xuser_is_current_user( user ) || GetTickCount64() >= deadline ) { hr = E_ABORT; break; }
                hr = GetBridge().Exchange( { "selection", id }, reply );
                if ( hr == E_PENDING ) { Sleep( 200 ); continue; }
                if ( SUCCEEDED( hr ) )
                {
                    if ( reply.size() != 1 ) hr = E_INVALIDARG;
                    else if ( canceled || !xuser_is_current_user( user ) ) hr = E_ABORT;
                    else hr = activity ? SendActivityInvite( GetNetwork(), user, xuser_get_xuid( user ), title, reply[0] ) : SendNetworkInvite( GetNetwork(), user, title, session, reply[0] );
                    GetBridge().Exchange( { "complete", id, std::to_string( static_cast<UINT32>( hr ) ) }, reply );
                }
                break;
            }
        }
        if ( FAILED( hr ) || canceled ) GetBridge().Exchange( { "cancel", id }, reply );
        return hr;
    }
};
static HRESULT StartRequest( XAsyncBlock *async, XUserHandle user, bool activity, std::vector<std::string> session )
{
    if ( !BridgeAvailable() ) return E_NOTIMPL;
    const auto *config = nexus::GetGameConfiguration(); if ( !config || !config->titleId || !xuser_is_current_user( user ) ) return E_ACCESSDENIED;
    try
    {
        auto request = std::make_shared<InviteRequest>(); request->activity = activity; request->title = config->titleId; request->session = std::move( session ); request->id = NewId();
        if ( request->id.empty() ) return E_FAIL;
        const auto hr = x_user_impl->XUserDuplicateHandle( user, &request->user ); if ( FAILED( hr ) ) return hr;
        return ScheduleDialog( async, activity, [request]( const std::atomic<bool> &canceled ) { return request->Run( canceled ); } );
    }
    catch ( ... ) { return E_OUTOFMEMORY; }
}
HRESULT ShowInvite( XAsyncBlock *async, XUserHandle user, const char *scid, const char *name, const char *session, const char *text, const char *context )
{
    if ( !async || !user || !scid || strlen( scid ) != 36 || !name || !*name || strlen( name ) > 100 || !session || !*session || strlen( session ) > 100 || ( text && strlen( text ) > 1024 ) || ( context && strlen( context ) > 1024 ) ) return E_INVALIDARG;
    try { return StartRequest( async, user, false, { scid, name, session, text ? text : "", context ? context : "" } ); }
    catch ( ... ) { return E_OUTOFMEMORY; }
}
HRESULT ShowInviteResult( XAsyncBlock *async ) { return nexus::GetAsyncStatusForIdentity( async, &inviteIdentity ); }
HRESULT ShowActivityInvite( XAsyncBlock *async, XUserHandle user ) { return !async || !user ? E_INVALIDARG : StartRequest( async, user, true, {} ); }
HRESULT ShowAchievements( XAsyncBlock *async, XUserHandle user, UINT32 title )
{
    if ( !async || !user ) return E_INVALIDARG;
    if ( !BridgeAvailable() ) return E_NOTIMPL;
    const auto *config = nexus::GetGameConfiguration(); if ( !config || !config->titleId || !xuser_is_current_user( user ) ) return E_ACCESSDENIED;
    if ( title && title != config->titleId ) return E_ACCESSDENIED;
    const auto resolved = config->titleId;
    return ScheduleDialog( async, false, [resolved]( const std::atomic<bool> &canceled ) {
        std::vector<std::string> reply;
        return canceled ? E_ABORT : GetBridge().Exchange( { "show-achievements", std::to_string( resolved ) }, reply );
    }, true );
}
HRESULT ShowAchievementsResult( XAsyncBlock *async ) { return nexus::GetAsyncStatusForIdentity( async, &achievementsIdentity ); }
HRESULT ShowActivityInviteResult( XAsyncBlock *async ) { return nexus::GetAsyncStatusForIdentity( async, &activityIdentity ); }
}
