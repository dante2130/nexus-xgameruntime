// SPDX-License-Identifier: MIT
// Closed transports and synthetic payloads only. Never calls XUserAdd.
#include "../../src/System/GameInvite/Network.h"
#include <algorithm>
#include <xgameevent.h>
#include <xgameui.h>
#include <cstdio>
#include <string>
#include <vector>
using namespace nexus::invites;
static unsigned checks, failures;
#define CHECK(label, expression) do { ++checks; if ( !( expression ) ) { ++failures; printf( "FAIL %s line=%u\n", label, __LINE__ ); } } while ( 0 )
struct Bridge final : Transport
{
    std::vector<std::vector<std::string>> sent;
    HRESULT Exchange( const std::vector<std::string> &request, std::vector<std::string> &reply ) noexcept override { sent.push_back( request ); reply.clear(); return S_OK; }
};
struct Network final : NetworkTransport
{
    std::vector<std::string> urls, pages;
    HRESULT result = S_OK;
    HRESULT Request( XUserHandle, const char *method, const std::string &url, const std::string &, const char *contract, DWORD &status, std::string &reply ) noexcept override
    {
        if ( std::string( method ) != "GET" || std::string( contract ) != "2" ) return E_INVALIDARG;
        urls.push_back( url ); status = 200; reply = pages.empty() ? "{\"achievements\":[]}" : pages[std::min( urls.size() - 1, pages.size() - 1 )]; return result;
    }
};
static std::string Item( const char *id, const char *state = "Achieved", const char *title = "123" )
{
    return std::string( "{\"id\":\"" ) + id + "\",\"serviceConfigId\":\"scid-1\",\"name\":\"Name " + id + "\",\"description\":\"Desc\",\"progressState\":\"" + state +
           "\",\"titleAssociations\":[{\"name\":\"Game\",\"id\":" + title + "}],\"mediaAssets\":[{\"name\":\"Icon\",\"type\":\"Icon\",\"url\":\"https://example.invalid/i.png\"}],"
           "\"rewards\":[{\"name\":\"\",\"description\":\"\",\"value\":\"25\",\"type\":\"Gamerscore\",\"valueType\":\"Int\"}]}";
}
static std::string Page( const std::string &items, const char *next = "null" )
{
    return "{\"achievements\":[" + items + "],\"pagingInfo\":{\"continuationToken\":" + next + "}}";
}
static void Push()
{
    std::vector<AchievementUnlock> got;
    const Achievement collect = [&]( const AchievementUnlock &unlock ) { got.push_back( unlock ); };
    const auto body = "{\"service\":\"achievements\",\"notificationData\":" + Item( "7" ) + "}";
    CHECK( "push-parsed", ParseAchievementNotification( body, 42, 123, collect ) == S_OK && got.size() == 1 );
    CHECK( "push-fields", got[0].id == "7" && got[0].scid == "scid-1" && got[0].name == "Name 7" && got[0].description == "Desc" && got[0].gamerscore == "25" && got[0].iconUrl == "https://example.invalid/i.png" && got[0].titleId == "123" );
    CHECK( "push-title-bound", FAILED( ParseAchievementNotification( body, 42, 124, collect ) ) && got.size() == 1 );
    CHECK( "push-other-service", FAILED( ParseAchievementNotification( "{\"service\":\"multiplayer\"}", 42, 123, collect ) ) );
    CHECK( "push-receiver-bound", FAILED( ParseAchievementNotification( "{\"service\":\"achievements\",\"xuid\":\"43\",\"notificationData\":" + Item( "7" ) + "}", 42, 123, collect ) ) );
    CHECK( "push-not-achieved", FAILED( ParseAchievementNotification( "{\"service\":\"achievements\",\"notificationData\":" + Item( "8", "InProgress" ) + "}", 42, 123, collect ) ) );
    CHECK( "push-malformed", FAILED( ParseAchievementNotification( "not JSON", 42, 123, collect ) ) && FAILED( ParseAchievementNotification( "{}", 42, 123, collect ) ) );
    CHECK( "invite-parser-still-rejects", FAILED( ParseNotification( body, 42, 123, []( const std::string &, const std::string &, const std::string &, UINT64 ) {} ) ) );
}
static void Message()
{
    AchievementUnlock unlock{ "123", "scid-1", "7", "Name", "Desc", "25", "https://example.invalid/i.png" };
    const auto request = AchievementRequest( unlock );
    CHECK( "contract-order", request == std::vector<std::string>{ "achievement-unlocked", "123", "scid-1", "7", "Name", "Desc", "25", "https://example.invalid/i.png" } );
    unlock.description = std::string( 20000, 'a' ); unlock.name = std::string( 1023, 'x' ) + "\xC3\xA9\xC3\xA9"; unlock.iconUrl = std::string( 9000, 'u' );
    const auto longer = AchievementRequest( unlock );
    CHECK( "contract-size", longer.size() == 8 && longer[5].size() == 4096 && longer[7].size() == 2048 );
    CHECK( "truncate-utf8-boundary", longer[4].size() == 1023 );
    unlock.name = std::string( "a\0b", 3 );
    CHECK( "nul-removed", AchievementRequest( unlock )[4] == "ab" );
    Bridge bridge; AchievementUnlock simple{ "123", "s", "1", "n", "d", "5", "u" };
    CHECK( "forward-sends", ForwardAchievement( bridge, simple ) == S_OK && bridge.sent.size() == 1 && bridge.sent[0][0] == "achievement-unlocked" && bridge.sent[0].size() == 8 );
}
static void Poll()
{
    Network network; AchievementWatcher watcher; std::vector<std::string> ids;
    const Achievement collect = [&]( const AchievementUnlock &unlock ) { ids.push_back( unlock.id ); };
    network.pages = { Page( Item( "1" ) + "," + Item( "2" ) ) };
    CHECK( "poll-seed", watcher.Poll( network, nullptr, 123, 42, 1000, collect ) == S_OK && ids.empty() );
    CHECK( "poll-url", network.urls[0].starts_with( "https://achievements.xboxlive.com/users/xuid(42)/achievements?titleId=123&unlockedOnly=true" ) );
    CHECK( "poll-throttled", watcher.Poll( network, nullptr, 123, 42, 2000, collect ) == S_FALSE && network.urls.size() == 1 );
    network.pages = { Page( Item( "1" ) + "," + Item( "2" ) + "," + Item( "3" ) ) };
    CHECK( "poll-diff", watcher.Poll( network, nullptr, 123, 42, 40000, collect ) == S_OK && ids == std::vector<std::string>{ "3" } );
    CHECK( "poll-dedupe", watcher.Poll( network, nullptr, 123, 42, 80000, collect ) == S_OK && ids.size() == 1 );
    network.pages = { Page( Item( "3" ) + "," + Item( "4", "Achieved", "999" ) + "," + Item( "5", "NotStarted" ) ) };
    CHECK( "poll-filters-other-title-and-locked", watcher.Poll( network, nullptr, 123, 42, 120000, collect ) == S_OK && ids.size() == 1 );
    const auto before = network.urls.size();
    network.pages = { Page( Item( "6" ), "\"tok\"" ), Page( Item( "7" ) ) };
    CHECK( "poll-paging", watcher.Poll( network, nullptr, 123, 42, 160000, collect ) == S_OK && network.urls.size() == before + 2 && network.urls.back().ends_with( "&continuationToken=tok" ) && ids == std::vector<std::string>{ "3", "6", "7" } );
    network.result = E_FAIL;
    CHECK( "poll-failure-reported", FAILED( watcher.Poll( network, nullptr, 123, 42, 200000, collect ) ) && ids.size() == 3 );
}
static void Dedupe()
{
    AchievementWatcher watcher; unsigned count = 0; const Achievement collect = [&]( const AchievementUnlock & ) { ++count; };
    AchievementUnlock unlock{ "123", "s", "1", "n", "d", "5", "u" };
    CHECK( "push-first-reported", watcher.Report( unlock, collect ) && count == 1 );
    CHECK( "push-duplicate-suppressed", !watcher.Report( unlock, collect ) && count == 1 );
    Network network; network.pages = { Page( Item( "1" ) + "," + Item( "9" ) ) };
    CHECK( "poll-seeds-after-push", watcher.Poll( network, nullptr, 123, 42, 1000, collect ) == S_OK && count == 1 );
    watcher.Reset();
    CHECK( "reset-forgets", watcher.Report( unlock, collect ) && count == 2 );
}
static void Stubs()
{
    IXGameUiImpl *ui = nullptr;
    CHECK( "ui-interface", SUCCEEDED( QueryApiImpl( CLSID_XGameUiImpl, __uuidof( IXGameUiImpl ), reinterpret_cast<void **>( &ui ) ) ) && ui );
    CHECK( "position-hint-ok", ui && ui->XGameUiSetNotificationPositionHint( XGameUiNotificationPositionHint_TopRight ) == S_OK );
    CHECK( "show-achievements-null-async", ui && ui->XGameUiShowAchievementsAsync( nullptr, nullptr, 123 ) == E_INVALIDARG );
    IXGameEventImpl *events = nullptr;
    CHECK( "event-interface", SUCCEEDED( QueryApiImpl( CLSID_XGameEventImpl, __uuidof( IXGameEventImpl ), reinterpret_cast<void **>( &events ) ) ) && events );
    CHECK( "event-write-rejects-null", events && events->XGameEventWrite( nullptr, "s", "p", "e", nullptr, nullptr ) == E_INVALIDARG );
    if ( ui ) ui->Release(); if ( events ) events->Release();
}
int main( int, char ** )
{
    InitializeLogging(); const auto com = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    Push(); Message(); Poll(); Dedupe(); Stubs();
    if ( SUCCEEDED( com ) ) CoUninitialize();
    printf( "CHECKS %u FAILURES %u\n", checks, failures ); return failures ? 1 : 0;
}
