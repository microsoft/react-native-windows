// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"

#include <Unicode.h>
#include <Utils.h>
#include "WinHTTPWebSocketResource.h"

// Windows API
#include <comutil.h>

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
  , m_errorStatus{ ERROR_SUCCESS }
{
#if 0 // WinHttpCrackUrl does not accept non-HTTP schemes 🙄
  BSTR bstr = _com_util::ConvertStringToBSTR(urlString.c_str());
  LPWSTR lpwstr = bstr;
  SysFreeString(bstr);

  LPURL_COMPONENTS url{};
  auto parseResult = WinHttpCrackUrl(lpwstr, static_cast<DWORD>(urlString.length()), /*flags*/ 0, url);
  if (!parseResult)
    throw std::exception("Could not parse URL.");

  if (wcscmp(L"wss", url->lpszScheme) == 0)
  {
    m_openSessionFlags |= WINHTTP_FLAG_SECURE;
  }
  else if (wcscmp(L"ws", url->lpszScheme) != 0)
  {
    throw std::exception(Common::Unicode::Utf16ToUtf8(
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
    m_errorStatus = GetLastError();

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
    m_errorStatus = GetLastError();

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
    m_errorStatus = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error starting request", ErrorType::Connection });
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
    m_errorStatus = GetLastError();
    throw std::exception("Can not create instance. Invalid callback.");
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
    m_errorStatus = GetLastError();

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
    0                               // dwContext
  );
  if (!fStatus)
  {
    m_errorStatus = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error sending handshake", ErrorType::Handshake });
}
  }
  fStatus = WinHttpReceiveResponse(m_requestHandle, /*lpReserved*/ 0);
  if (!fStatus)
  {
    m_errorStatus = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error receiving handshake", ErrorType::Handshake });
    }
  }

  // Complete upgrade. Accept only HTTP status code 101.
  m_webSocketHandle = WinHttpWebSocketCompleteUpgrade(m_requestHandle, /*pContext*/ NULL);
  if (m_webSocketHandle == NULL)
  {
    m_errorStatus = GetLastError();

    if (m_errorHandler)
    {
      return m_errorHandler({ "Error completing protocol upgrade", ErrorType::Connection });
    }
  }

  WinHttpCloseHandle(m_requestHandle);
  m_requestHandle = NULL;
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
}

IWebSocketResource::ReadyState WinHTTPWebSocketResource::GetReadyState() const
{
  return ReadyState::Closed;
}

void WinHTTPWebSocketResource::SetOnConnect(function<void()>&& handler)
{
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
}

void WinHTTPWebSocketResource::SetOnError(function<void(Error&&)>&& handler)
{
}

#pragma endregion IWebSocketResource

#pragma region Private members

/*static*/ VOID CALLBACK WinHTTPWebSocketResource::Callback(HINTERNET handle, DWORD_PTR context, DWORD status, void* info, DWORD infoLength)
{
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
    break;
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
