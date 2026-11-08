// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once
#include "Bridge.h"
#include <functional>
#include <memory>
#include <mutex>
#include <set>
namespace nexus::invites
{
using Invitation = std::function<void( const std::string &, const std::string &, const std::string &, UINT64 )>;
class NetworkTransport
{
public:
    virtual ~NetworkTransport() = default;
    virtual HRESULT Request( XUserHandle user, const char *method, const std::string &url, const std::string &body, const char *contract, DWORD &status, std::string &reply ) noexcept = 0;
};
NetworkTransport &GetNetwork() noexcept;
HRESULT SendNetworkInvite( NetworkTransport &network, XUserHandle user, UINT32 title, const std::vector<std::string> &session, const std::string &recipient ) noexcept;
HRESULT SendActivityInvite( NetworkTransport &network, XUserHandle user, UINT64 owner, UINT32 title, const std::string &recipient ) noexcept;
struct AchievementUnlock { std::string titleId, scid, id, name, description, gamerscore, iconUrl; };
using Achievement = std::function<void( const AchievementUnlock & )>;
// Bridge request in the fixed order: achievement-unlocked, titleId, scid, achievementId, name, description, gamerscore, iconUrl.
std::vector<std::string> AchievementRequest( const AchievementUnlock &unlock );
HRESULT ParseAchievementNotification( const std::string &body, UINT64 receiver, UINT32 title, const Achievement &callback ) noexcept;
HRESULT ParseAchievements( const std::string &body, UINT32 title, std::vector<AchievementUnlock> &unlocks, std::string &next ) noexcept;
// Shared by the push and poll paths so each unlock is announced once. The first
// successful poll only seeds the baseline of already-unlocked achievements.
class AchievementWatcher
{
    std::mutex lock;
    std::set<std::string> seen;
    bool seeded = false;
    ULONGLONG nextPoll = 0;
public:
    bool Report( const AchievementUnlock &unlock, const Achievement &callback ) noexcept;
    HRESULT Poll( NetworkTransport &network, XUserHandle user, UINT32 title, UINT64 xuid, ULONGLONG now, const Achievement &callback ) noexcept;
    void Reset() noexcept;
};
AchievementWatcher &GetAchievementWatcher() noexcept;
// Sends the unlock to the launcher; true when the bridge accepted it.
HRESULT ForwardAchievement( Transport &bridge, const AchievementUnlock &unlock ) noexcept;
void StartAchievements( XUserHandle user ) noexcept;
void StopAchievements() noexcept;
HRESULT ParseNotification( const std::string &body, UINT64 receiver, UINT32 title, const Invitation &callback ) noexcept;
class WebSocketReader
{
    struct State;
    std::unique_ptr<State> state;
    static void CALLBACK Status( HINTERNET, DWORD_PTR, DWORD, void *, DWORD );
public:
    WebSocketReader();
    ~WebSocketReader();
    HRESULT Open( HINTERNET connection, const WCHAR *path, DWORD flags, const std::wstring &headers ) noexcept;
    HRESULT Send( const std::string &frame ) noexcept;
    HRESULT Next( std::string &frame ) noexcept;
    void Stop() noexcept;
};
class Notifications
{
    struct State;
    std::unique_ptr<State> state;
public:
    Notifications();
    ~Notifications();
    void Tick( XUserHandle user, UINT32 title, const Invitation &callback, const Achievement &achievement = {} ) noexcept;
    bool Connected() const noexcept;
    void Close() noexcept;
};
}
