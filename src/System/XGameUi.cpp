/*
 * Xbox Game runtime Library
 *  GDK Component: System API -> XGameUI
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

#include "XGameUi.h"
#include "GameInvite/Events.h"

HRESULT WINAPI XGameUiImpl::XGameUiShowMessageDialogAsync( XAsyncBlock *async, const char *titleText,
                                                           const char *contentText, const char *firstButtonText,
                                                           const char *secondButtonText, const char *thirdButtonText,
                                                           XGameUiMessageDialogButton defaultButton,
                                                           XGameUiMessageDialogButton cancelButton )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowMessageDialogResult( XAsyncBlock *async,
                                                            XGameUiMessageDialogButton *resultButton )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowSendGameInviteAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                            const char *sessionConfigurationId,
                                                            const char *sessionTemplateName, const char *sessionId,
                                                            const char *invitationText,
                                                            const char *customActivationContext )
{
    ( void )0;
    return nexus::invites::BridgeAvailable() ? nexus::invites::ShowInvite( async, requestingUser, sessionConfigurationId, sessionTemplateName, sessionId, invitationText, customActivationContext ) : E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowSendGameInviteResult( XAsyncBlock *async )
{
    ( void )0;
    return nexus::invites::BridgeAvailable() ? nexus::invites::ShowInviteResult( async ) : E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowPlayerProfileCardAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                               UINT64 targetPlayer )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowPlayerProfileCardResult( XAsyncBlock *async )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowAchievementsAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                          UINT32 titleId )
{
    ( void )0;
    return nexus::invites::ShowAchievements( async, requestingUser, titleId );
}

HRESULT WINAPI XGameUiImpl::XGameUiShowAchievementsResult( XAsyncBlock *async )
{
    ( void )0;
    return nexus::invites::BridgeAvailable() ? nexus::invites::ShowAchievementsResult( async ) : E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowPlayerPickerAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                          const char *promptText, UINT32 selectFromPlayersCount,
                                                          const UINT64 *selectFromPlayers,
                                                          UINT32 preSelectedPlayersCount, UINT64 *preSelectedPlayers,
                                                          UINT32 minSelectionCount, UINT32 maxSelectionCount )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowPlayerPickerResultCount( XAsyncBlock *async, UINT32 *resultPlayersCount )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowPlayerPickerResult( XAsyncBlock *async, UINT32 resultPlayersCount,
                                                           UINT64 *resultPlayers, UINT32 *resultPlayersUsed )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowErrorDialogAsync( XAsyncBlock *async, HRESULT errorCode, const char *context )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowErrorDialogResult( XAsyncBlock *async )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetNotificationPositionHint( XGameUiNotificationPositionHint position )
{
    ( void )0;
    notificationPosition = position;
    return S_OK;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowTextEntryAsync( XAsyncBlock *async, const char *titleText,
                                                       const char *descriptionText, const char *defaultText,
                                                       XGameUiTextEntryInputScope inputScope, UINT32 maxTextLength )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowTextEntryResultSize( XAsyncBlock *async, UINT32 *resultTextBufferSize )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowTextEntryResult( XAsyncBlock *async, UINT32 resultTextBufferSize,
                                                        char *resultTextBuffer, UINT32 *resultTextBufferUsed )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING_2__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING_3__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING_4__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowWebAuthenticationAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                               const char *requestUri, const char *completionUri )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowWebAuthenticationResultSize( XAsyncBlock *async, SIZE_T *bufferSize )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowWebAuthenticationResult( XAsyncBlock *async, SIZE_T bufferSize, void *buffer,
                                                                XGameUiWebAuthenticationResultData **ptrToBuffer,
                                                                SIZE_T *bufferUsed )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowWebAuthenticationWithOptionsAsync( XAsyncBlock *async,
                                                                          XUserHandle requestingUser,
                                                                          const char *requestUri,
                                                                          const char *completionUri,
                                                                          XGameUiWebAuthenticationOptions options )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING_5__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING_6__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowMultiplayerActivityGameInviteAsync( XAsyncBlock *async,
                                                                           XUserHandle requestingUser )
{
    ( void )0;
    return nexus::invites::BridgeAvailable() ? nexus::invites::ShowActivityInvite( async, requestingUser ) : E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowMultiplayerActivityGameInviteResult( XAsyncBlock *async )
{
    ( void )0;
    return nexus::invites::BridgeAvailable() ? nexus::invites::ShowActivityInviteResult( async ) : E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING_7__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::__PADDING_8__()
{
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiTextEntryOpen( const XGameUiTextEntryOptions *options, UINT32 maxLength,
                                                  const char *initialText, UINT32 initialCursorIndex,
                                                  XGameUiTextEntryHandle *handle )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiTextEntryClose( XGameUiTextEntryHandle handle )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiTextEntryGetState( XGameUiTextEntryHandle handle,
                                                      XGameUiTextEntryChangeTypeFlags *changeType, UINT32 *cursorIndex,
                                                      UINT32 *imeClauseStartIndex, UINT32 *imeClauseEndIndex,
                                                      UINT32 bufferSize, char *buffer )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiTextEntryGetExtents( XGameUiTextEntryHandle handle,
                                                        XGameUiTextEntryExtents *extents )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiTextEntryUpdatePositionHint( XGameUiTextEntryHandle handle,
                                                                XGameUiTextEntryPositionHint positionHint )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiTextEntryUpdateVisibility( XGameUiTextEntryHandle handle,
                                                              XGameUiTextEntryVisibilityFlags visibilityFlags )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowStateShareAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                        const char *linkToken )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiShowStateShareResult( XAsyncBlock *async )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetUiCallbacks( const XGameUiUiCallbacks *callbacks, BOOLEAN useSystemUiIfAvailable )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetMessageDialogUiResponse( XGameUiCallbackHandle callbackHandle,
                                                               XGameUiMessageDialogButton response )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetPlayerPickerUiResponse( XGameUiCallbackHandle callbackHandle, UINT32 playerCount,
                                                              const UINT64 *players )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetTextEntryUiResponse( XGameUiCallbackHandle callbackHandle, const char *response )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetPlayerProfileCardUiResponse( XGameUiCallbackHandle callbackHandle )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetSendGameInviteUiResponse( XGameUiCallbackHandle callbackHandle )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetAchievementsUiResponse( XGameUiCallbackHandle callbackHandle )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetMultiplayerActivityGameInviteUiResponse( XGameUiCallbackHandle callbackHandle )
{
    ( void )0;
    return E_NOTIMPL;
}

HRESULT WINAPI XGameUiImpl::XGameUiSetErrorDialogUiResponse( XGameUiCallbackHandle callbackHandle )
{
    ( void )0;
    return E_NOTIMPL;
}

IUnknown *GetGameUiInterface() noexcept
{
    static XGameUiImpl value;
    return &value;
}
