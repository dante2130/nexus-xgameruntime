/*
 * Xbox Game runtime Library
 *  GDK Component: System API -> XGameEvent
 *
 * Copyright 2026 Olivia Ryan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "XGameEvent.h"
#include "GameInvite/Bridge.h"

HRESULT WINAPI XGameEventImpl::XGameEventWrite( XUserHandle user, const char *serviceConfigId,
                                                const char *playSessionId, const char *eventName,
                                                const char *dimensionsJson, const char *measurementsJson )
{
    ( void )0;
    if ( !user || !serviceConfigId || !playSessionId || !eventName || !*eventName ) return E_INVALIDARG;
    const char *const fields[] = { serviceConfigId, playSessionId, eventName, dimensionsJson ? dimensionsJson : "", measurementsJson ? measurementsJson : "" };
    for ( const auto *field : fields ) if ( strlen( field ) > 4096 ) return E_INVALIDARG;
    // Telemetry is best effort: a missing launcher bridge must not fail the title.
    if ( !nexus::invites::BridgeAvailable() ) return S_OK;
    try
    {
        std::vector<std::string> reply;
        nexus::invites::GetBridge().Exchange( { "game-event", std::to_string( xuser_get_xuid( user ) ), fields[0], fields[1], fields[2], fields[3], fields[4] }, reply );
    }
    catch ( ... ) {}
    return S_OK;
}

IUnknown *GetGameEventInterface() noexcept
{
    static XGameEventImpl value;
    return &value;
}
