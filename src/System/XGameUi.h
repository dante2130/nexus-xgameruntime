// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once
#include "../private.h"
#include "StaticComObject.h"
#include <xgameui.h>
#include <atomic>
class XGameUiImpl final : public nexus::StaticComObject<IXGameUiImpl4, IXGameUiImpl3, IXGameUiImpl2, IXGameUiImpl>
{
public:
    HRESULT WINAPI XGameUiShowMessageDialogAsync( XAsyncBlock *async, const char *titleText, const char *contentText,
                                                  const char *firstButtonText, const char *secondButtonText,
                                                  const char *thirdButtonText, XGameUiMessageDialogButton defaultButton,
                                                  XGameUiMessageDialogButton cancelButton ) override;
    HRESULT WINAPI XGameUiShowMessageDialogResult( XAsyncBlock *async,
                                                   XGameUiMessageDialogButton *resultButton ) override;
    HRESULT WINAPI XGameUiShowSendGameInviteAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                   const char *sessionConfigurationId, const char *sessionTemplateName,
                                                   const char *sessionId, const char *invitationText,
                                                   const char *customActivationContext ) override;
    HRESULT WINAPI XGameUiShowSendGameInviteResult( XAsyncBlock *async ) override;
    HRESULT WINAPI XGameUiShowPlayerProfileCardAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                      UINT64 targetPlayer ) override;
    HRESULT WINAPI XGameUiShowPlayerProfileCardResult( XAsyncBlock *async ) override;
    HRESULT WINAPI XGameUiShowAchievementsAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                 UINT32 titleId ) override;
    HRESULT WINAPI XGameUiShowAchievementsResult( XAsyncBlock *async ) override;
    HRESULT WINAPI XGameUiShowPlayerPickerAsync( XAsyncBlock *async, XUserHandle requestingUser, const char *promptText,
                                                 UINT32 selectFromPlayersCount, const UINT64 *selectFromPlayers,
                                                 UINT32 preSelectedPlayersCount, UINT64 *preSelectedPlayers,
                                                 UINT32 minSelectionCount, UINT32 maxSelectionCount ) override;
    HRESULT WINAPI XGameUiShowPlayerPickerResultCount( XAsyncBlock *async, UINT32 *resultPlayersCount ) override;
    HRESULT WINAPI XGameUiShowPlayerPickerResult( XAsyncBlock *async, UINT32 resultPlayersCount, UINT64 *resultPlayers,
                                                  UINT32 *resultPlayersUsed ) override;
    HRESULT WINAPI XGameUiShowErrorDialogAsync( XAsyncBlock *async, HRESULT errorCode, const char *context ) override;
    HRESULT WINAPI XGameUiShowErrorDialogResult( XAsyncBlock *async ) override;
    HRESULT WINAPI XGameUiSetNotificationPositionHint( XGameUiNotificationPositionHint position ) override;
    HRESULT WINAPI XGameUiShowTextEntryAsync( XAsyncBlock *async, const char *titleText, const char *descriptionText,
                                              const char *defaultText, XGameUiTextEntryInputScope inputScope,
                                              UINT32 maxTextLength ) override;
    HRESULT WINAPI XGameUiShowTextEntryResultSize( XAsyncBlock *async, UINT32 *resultTextBufferSize ) override;
    HRESULT WINAPI XGameUiShowTextEntryResult( XAsyncBlock *async, UINT32 resultTextBufferSize, char *resultTextBuffer,
                                               UINT32 *resultTextBufferUsed ) override;
    HRESULT WINAPI __PADDING__() override;
    HRESULT WINAPI __PADDING_2__() override;
    HRESULT WINAPI __PADDING_3__() override;
    HRESULT WINAPI __PADDING_4__() override;
    HRESULT WINAPI XGameUiShowWebAuthenticationAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                      const char *requestUri, const char *completionUri ) override;
    HRESULT WINAPI XGameUiShowWebAuthenticationResultSize( XAsyncBlock *async, SIZE_T *bufferSize ) override;
    HRESULT WINAPI XGameUiShowWebAuthenticationResult( XAsyncBlock *async, SIZE_T bufferSize, void *buffer,
                                                       XGameUiWebAuthenticationResultData **ptrToBuffer,
                                                       SIZE_T *bufferUsed ) override;
    HRESULT WINAPI XGameUiShowWebAuthenticationWithOptionsAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                                                 const char *requestUri, const char *completionUri,
                                                                 XGameUiWebAuthenticationOptions options ) override;
    HRESULT WINAPI __PADDING_5__() override;
    HRESULT WINAPI __PADDING_6__() override;
    HRESULT WINAPI XGameUiShowMultiplayerActivityGameInviteAsync( XAsyncBlock *async,
                                                                  XUserHandle requestingUser ) override;
    HRESULT WINAPI XGameUiShowMultiplayerActivityGameInviteResult( XAsyncBlock *async ) override;
    HRESULT WINAPI __PADDING_7__() override;
    HRESULT WINAPI __PADDING_8__() override;
    HRESULT WINAPI XGameUiTextEntryOpen( const XGameUiTextEntryOptions *options, UINT32 maxLength,
                                         const char *initialText, UINT32 initialCursorIndex,
                                         XGameUiTextEntryHandle *handle ) override;
    HRESULT WINAPI XGameUiTextEntryClose( XGameUiTextEntryHandle handle ) override;
    HRESULT WINAPI XGameUiTextEntryGetState( XGameUiTextEntryHandle handle, XGameUiTextEntryChangeTypeFlags *changeType,
                                             UINT32 *cursorIndex, UINT32 *imeClauseStartIndex,
                                             UINT32 *imeClauseEndIndex, UINT32 bufferSize, char *buffer ) override;
    HRESULT WINAPI XGameUiTextEntryGetExtents( XGameUiTextEntryHandle handle,
                                               XGameUiTextEntryExtents *extents ) override;
    HRESULT WINAPI XGameUiTextEntryUpdatePositionHint( XGameUiTextEntryHandle handle,
                                                       XGameUiTextEntryPositionHint positionHint ) override;
    HRESULT WINAPI XGameUiTextEntryUpdateVisibility( XGameUiTextEntryHandle handle,
                                                     XGameUiTextEntryVisibilityFlags visibilityFlags ) override;
    HRESULT WINAPI XGameUiShowStateShareAsync( XAsyncBlock *async, XUserHandle requestingUser,
                                               const char *linkToken ) override;
    HRESULT WINAPI XGameUiShowStateShareResult( XAsyncBlock *async ) override;
    HRESULT WINAPI XGameUiSetUiCallbacks( const XGameUiUiCallbacks *callbacks,
                                          BOOLEAN useSystemUiIfAvailable ) override;
    HRESULT WINAPI XGameUiSetMessageDialogUiResponse( XGameUiCallbackHandle callbackHandle,
                                                      XGameUiMessageDialogButton response ) override;
    HRESULT WINAPI XGameUiSetPlayerPickerUiResponse( XGameUiCallbackHandle callbackHandle, UINT32 playerCount,
                                                     const UINT64 *players ) override;
    HRESULT WINAPI XGameUiSetTextEntryUiResponse( XGameUiCallbackHandle callbackHandle, const char *response ) override;
    HRESULT WINAPI XGameUiSetPlayerProfileCardUiResponse( XGameUiCallbackHandle callbackHandle ) override;
    HRESULT WINAPI XGameUiSetSendGameInviteUiResponse( XGameUiCallbackHandle callbackHandle ) override;
    HRESULT WINAPI XGameUiSetAchievementsUiResponse( XGameUiCallbackHandle callbackHandle ) override;
    HRESULT WINAPI XGameUiSetMultiplayerActivityGameInviteUiResponse( XGameUiCallbackHandle callbackHandle ) override;
    HRESULT WINAPI XGameUiSetErrorDialogUiResponse( XGameUiCallbackHandle callbackHandle ) override;
    XGameUiNotificationPositionHint GetNotificationPositionHint() const noexcept { return static_cast<XGameUiNotificationPositionHint>( notificationPosition.load() ); }

private:
    std::atomic<UINT32> notificationPosition{ 0 };
};
IUnknown *GetGameUiInterface() noexcept;
