// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <map>
#include <vector>

namespace Microsoft {
namespace React {

struct IWebSocket
{
  #pragma region Aliases

  using Protocols = std::vector<std::string>;
  using Options = std::map<std::wstring, std::string>;

  #pragma endregion // Aliases

  #pragma region Inner types

  enum class ReadyState : std::uint16_t
  {
    Connecting = 0, // Handle initialized
    Open = 1, // Ready to send
    Closing = 2, // Currently closing
    Closed = 3, // Closed or failed to open
    Size = 4
  };

  enum class ErrorType : size_t
  {
    None = 0,
    Resolution = 1,
    Connection = 2,
    Handshake = 3,
    Ping = 4,
    Send = 5,
    Receive = 6,
    Close = 7,
    Size = 8
  };

  enum class CloseCode : std::uint16_t
  {
    // Keep in sync with RFC 6455 specification
    None = 0,
    Normal = 1000,
    GoingAway = 1001,
    ProtocolError = 1002,
    UnknownData = 1003,
    Reserved1 = 1004,
    NoStatus = 1005,
    Abnormal = 1006,
    BadPayload = 1007,
    PolicyError = 1008,
    TooBig = 1009,
    NeedsExtension = 1010,
    InternalError = 1011,
    ServiceRestart = 1012,
    TryAgainLater = 1013,
    Reserved2 = 1014,
    Reserved3 = 1015,
  };

  struct Error
  {
    std::string Message;
    const ErrorType Type;
  };

  #pragma endregion // Inner types

  static std::unique_ptr<IWebSocket> __cdecl Make(const std::string& url);

  // Only use if a legacy implementation is required as fallback.
  static std::unique_ptr<IWebSocket> __cdecl MakeLegacy(const std::string& url);

  virtual ~IWebSocket() {}

  ///
  // Parameters:
  //
  // protocols - vector of protocols
  // options   - map of headers
  ///
  virtual void Connect(const Protocols& protocols = {}, const Options& options = {}) = 0;
  virtual void Ping() = 0;
  virtual void Send(const std::string& message) = 0;
  virtual void SendBinary(const std::string& base64String) = 0;
  virtual void Close(CloseCode code, const std::string& reason) = 0;

  virtual ReadyState GetReadyState() const = 0;

  virtual void SetOnConnect(std::function<void()>&& handler) = 0;
  virtual void SetOnPing(std::function<void()>&& handler) = 0;
  virtual void SetOnSend(std::function<void(std::size_t)>&& handler) = 0;
  virtual void SetOnMessage(std::function<void(std::size_t, const std::string&)>&& handler) = 0;
  virtual void SetOnClose(std::function<void(CloseCode, const std::string&)>&& handler) = 0;
  virtual void SetOnError(std::function<void(Error&&)>&& handler) = 0;
};

} } // namespace facebook::react

// Deprecated. Keeping for compatibility with dependent code.
namespace facebook {
namespace react {

using IWebSocket = Microsoft::React::IWebSocket;

} } // namespace facebook::react
