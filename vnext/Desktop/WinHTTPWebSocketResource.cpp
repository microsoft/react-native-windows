// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"

#include <Unicode.h>
#include <Utils.h>
#include "WinHTTPWebSocketResource.h"

// Windows API
#include <comutil.h>

using Microsoft::Common::Unicode::Utf8ToUtf16;

using std::function;
using std::size_t;
using std::string;
using std::wstring;

namespace Microsoft::React
{

WinHTTPWebSocketResource::WinHTTPWebSocketResource(const string& urlString)
  : m_openSessionFlags{ WINHTTP_FLAG_ASYNC }
  , m_sessionHandle{ NULL }
  , m_connectionHandle{ NULL }
  , m_requestHandle{ NULL }
  , m_lastError{ ERROR_SUCCESS }
{
#if 0 // WinHttpCrackUrl does not accept non-HTTP
  LPURL_COMPONENTS url{};
  auto parseResult = WinHttpCrackUrl(Utf8ToUtf16(urlString).c_str(), static_cast<DWORD>(urlString.length()), /*flags*/ 0, url);
  if (!parseResult)
    throw std::invalid_argument("Could not parse URL.");

  if (wcscmp(L"wss", url->lpszScheme) == 0)
  {
    m_openSessionFlags |= WINHTTP_FLAG_SECURE;
  }
  else if (wcscmp(L"ws", url->lpszScheme) != 0)
  {
    throw std::invalid_argument(Common::Unicode::Utf16ToUtf8(
      std::wstring(L"Incorrect URL scheme: ") + std::wstring(url->lpszScheme)
    ).c_str());
  }

  m_url = *url;
#else
  Url url{ urlString };
  ZeroMemory(&m_url, sizeof(m_url));
  m_url.dwStructSize = sizeof(m_url);
  m_url.lpszScheme = _wcsdup(wstring(url.scheme.begin(), url.scheme.end()).c_str());
  m_url.dwSchemeLength = static_cast<DWORD>(url.scheme.length());
  m_url.lpszHostName = _wcsdup(wstring(url.host.begin(), url.host.end()).c_str());
  m_url.dwHostNameLength = static_cast<DWORD>(url.host.length());
  m_url.nPort = static_cast<INTERNET_PORT>(std::atoi(url.port.c_str()));
  m_url.lpszUrlPath = _wcsdup(wstring(url.path.begin(), url.path.end()).c_str());
  m_url.dwUrlPathLength = static_cast<DWORD>(url.path.length());
  m_url.lpszUserName = NULL;
  m_url.dwUserNameLength = 0;
  m_url.lpszPassword = NULL;
  m_url.dwPasswordLength = 0;
  m_url.lpszExtraInfo = NULL;
  m_url.dwExtraInfoLength = 0;
#endif // 0
}

WinHTTPWebSocketResource::~WinHTTPWebSocketResource() /*override*/
{
  if (m_webSocketHandle != NULL)
  {
    WinHttpCloseHandle(m_webSocketHandle);
    m_webSocketHandle = NULL;
  }

  if (m_requestHandle != NULL)
  {
    WinHttpCloseHandle(m_requestHandle);
    m_requestHandle = NULL;
  }

  if (m_connectionHandle != NULL)
  {
    WinHttpCloseHandle(m_connectionHandle);
    m_connectionHandle = NULL;
  }

  if (m_sessionHandle != NULL)
  {
    WinHttpCloseHandle(m_sessionHandle);
    m_sessionHandle = NULL;
  }
}

#pragma region IWebSocketResource

void WinHTTPWebSocketResource::Connect(const Protocols& protocols, const Options& options)
{
  m_sessionHandle = WinHttpOpen(
    L"React Native Windows",              // pszAgentW
    WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,  // dwAccessType
    NULL,                                 // pszProxyW
    NULL,                                 // pszProxyBypassW
    m_openSessionFlags                    // dwFlags
  );

  if (m_sessionHandle == NULL)
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error starting session", ErrorType::Connection });
    }
  }

  m_connectionHandle = WinHttpConnect(
    m_sessionHandle,    // hSession
    m_url.lpszHostName, // pswzServerName
    m_url.nPort,        // nServerPort
    0                   // dwReserved
  );

  if (m_connectionHandle == NULL)
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error opening connection", ErrorType::Connection });
    }
  }

  m_requestHandle = WinHttpOpenRequest(
    m_connectionHandle, // hConnect
    L"GET",             // pwszVerb
    m_url.lpszUrlPath,  // pwszObjectName
    NULL,               // pwszVersion
    NULL,               // pwszReferrer
    NULL,               // ppwszAcceptTypes
    0                   // dwFlags
  );

  if (m_requestHandle == NULL)
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error starting request", ErrorType::Connection });
    }
  }

  if (!WinHttpSetOption(m_requestHandle, WINHTTP_OPTION_CONTEXT_VALUE, this, sizeof(this)))
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error configuring request", ErrorType::Connection });
    }
  }

  // Set WinHTTP callbacks
  if (WinHttpSetStatusCallback(
    m_requestHandle,                          // hInternet
    Callback,                                 // lpfnInternetCallback
    WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,  // dwNotificationFlags
    0                                         // dwReserved
  ) == WINHTTP_INVALID_STATUS_CALLBACK)
  {
    m_lastError = GetLastError();
    throw std::exception("Cannot create instance. Invalid callback.");
  }

  BOOL fStatus = FALSE; //TODO: Make member?
  fStatus = WinHttpSetOption(
    m_requestHandle,                      // hInternet
    WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, // dwOption
    NULL,                                 // lpBuffer
    0                                     // dwBufferLength
  );

  if (!fStatus)
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error upgrading protocol", ErrorType::Connection });
    }
  }

  // Perform handshake
  fStatus = WinHttpSendRequest(
    m_requestHandle,                // hRequest
    WINHTTP_NO_ADDITIONAL_HEADERS,  // lpszHeaders
    0,                              // dwHeadersLength
    NULL,                           // lpOptional
    0,                              // dwOptionalLength
    0,                              // dwTotalLength
    (DWORD_PTR)this                 // dwContext
  );
  if (!fStatus)
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error sending handshake", ErrorType::Handshake });
    }
  }
  fStatus = WinHttpReceiveResponse(m_requestHandle, /*lpReserved*/ 0);
  if (!fStatus)
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error receiving handshake", ErrorType::Handshake });
    }
  }

  // Complete upgrade. Accept only HTTP status code 101.
  m_webSocketHandle = WinHttpWebSocketCompleteUpgrade(m_requestHandle, /*pContext*/ NULL);
  if (m_webSocketHandle == NULL)
  {
    m_lastError = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error completing protocol upgrade", ErrorType::Connection });
    }
  }

  // Connection succeeded.
  if (m_connectHandler)
  {
    m_connectHandler();
  }
}

void WinHTTPWebSocketResource::Ping() {}

void WinHTTPWebSocketResource::Send(const string& message)
{
}

void WinHTTPWebSocketResource::SendBinary(const string& base64String)
{
}

void WinHTTPWebSocketResource::Close(CloseCode code, const string& reason)
{
  auto result = WinHttpWebSocketShutdown(
    m_webSocketHandle,                                         // hWebSocket
    static_cast<USHORT>(code),                                 // usStatus
    static_cast<LPWSTR>(_wcsdup(Utf8ToUtf16(reason).c_str())), // pvReason
    static_cast<DWORD>(reason.length())                        // dwReasonLength
  );
  switch (result)
  {
  case ERROR_IO_PENDING:
    // Success (finishing asynchronously)
  default:
    // Success
    break;
  }

  result = WinHttpWebSocketClose(
    m_webSocketHandle,                                        // hWebSocket
    static_cast<USHORT>(code),                                // usStatus
    static_cast<PVOID>(_wcsdup(Utf8ToUtf16(reason).c_str())), // pvReason
    static_cast<DWORD>(reason.length())                       // dwReasonLength
  );
  switch (result)
  {
  case ERROR_INVALID_OPERATION:
  case ERROR_INVALID_PARAMETER:
  case 12152:// ERROR_INVALID_SERVER_RESPONSE
    if (m_errorHandler)
    {
      m_errorHandler({ "Failed closing WebSocket", ErrorType::Close });
    }
    break;

  default:
    // Success
    if (m_closeHandler)
    {
      m_closeHandler(CloseCode::Normal, "Closing");
    }
    break;
  }
}

IWebSocketResource::ReadyState WinHTTPWebSocketResource::GetReadyState() const
{
  return ReadyState::Closed;
}

void WinHTTPWebSocketResource::SetOnConnect(function<void()>&& handler)
{
  m_connectHandler = std::move(handler);
}

void WinHTTPWebSocketResource::SetOnPing(function<void()>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnSend(function<void(size_t)>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnMessage(function<void(size_t, const string&)>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnClose(function<void(CloseCode, const string&)>&& handler)
{
  m_closeHandler = std::move(handler);
}

void WinHTTPWebSocketResource::SetOnError(function<void(Error&&)>&& handler)
{
  m_errorHandler = std::move(handler);
}

#pragma endregion IWebSocketResource

#pragma region Private members

/*static*/ VOID CALLBACK WinHTTPWebSocketResource::Callback(HINTERNET handle, DWORD_PTR context, DWORD status, LPVOID info, DWORD infoLength)
{
  auto pThis = (WinHTTPWebSocketResource*)context;//TODO: Handle/cast correctly.

  switch (status)
  {
  case WINHTTP_CALLBACK_STATUS_RESOLVING_NAME:
    break;
  case WINHTTP_CALLBACK_STATUS_NAME_RESOLVED:
    break;
  case WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER:
    break;
  case WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER:
    break;
  case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST:
    break;
  case WINHTTP_CALLBACK_STATUS_REQUEST_SENT:
    break;
  case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE:
    break;
  case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
    break;
  case WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION:
    break;
  case WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED:
    break;
  case WINHTTP_CALLBACK_STATUS_HANDLE_CREATED:
    break;
  case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
    break;
  case WINHTTP_CALLBACK_STATUS_DETECTING_PROXY:
    break;
  case WINHTTP_CALLBACK_STATUS_REDIRECT:
    break;
  case WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE:
    break;
  case WINHTTP_CALLBACK_STATUS_SECURE_FAILURE:
    break;
  case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
    break;
  case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    break;
  case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    break;
  case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
    break;
  case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    pThis->m_lastError = GetLastError();
    if (pThis->m_errorHandler)
      pThis->m_errorHandler({ "REQUEST_ERROR", ErrorType::None });

    return;
  case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    break;
  case WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE:
    break;
  case WINHTTP_CALLBACK_STATUS_CLOSE_COMPLETE:
    break;
  case WINHTTP_CALLBACK_STATUS_SHUTDOWN_COMPLETE:
    break;
  case WINHTTP_CALLBACK_STATUS_SETTINGS_WRITE_COMPLETE:
    break;
  case WINHTTP_CALLBACK_STATUS_SETTINGS_READ_COMPLETE:
    break;
  default:
    break;
  }
}

#pragma endregion Private members

} // namespace Microsoft::React
