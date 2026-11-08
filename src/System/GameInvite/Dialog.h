// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once
#include "Bridge.h"
#include <atomic>
#include <functional>
namespace nexus::invites
{
// Internal async UI scheduling. The task owns any request/user resources and
// must finish before the caller's block is completed. Not a DLL export.
using DialogTask = std::function<HRESULT( const std::atomic<bool> & )>;
HRESULT ScheduleDialog( XAsyncBlock *async, bool activity, DialogTask task, bool achievements = false );
}
