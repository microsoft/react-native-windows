// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <map>
#include <vector>

namespace Microsoft::React {

///
/// Defines the core functionality for a native WebSocket client resource.
///
struct IWebSocket
{
  #pragma region Aliases

  using Protocols = std::vector<std::string>;
  using Options = std::map<std::wstring, std::string>;

  #pragma endregion // Aliases

  #pragma region Inner types

  /// <summary>
  /// As defined in RFC6455.
  /// </summary>
  enum class ReadyState : std::uint16_t
  {
    Connecting = 0, // Handle initialized
    Open = 1,       // Ready to send
    Closing = 2,    // Currently closing
    Closed = 3,     // Closed or failed to open
    Size = 4        // Metavalue representing the number of entries in this enum.
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
    Size = 8 // Metavalue representing the number of entries in this enum.
  };

  /// <summary>
  /// As defined in https://tools.ietf.org/html/rfc6455#section-7.4
  /// </summary>
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

  /// <summary>
  /// Creates an <c>IWebSocket</c> instance.
  /// </summary>
  /// <param name="url">
  /// WebSocket URL address the instance will connect to.
  /// The address's scheme can be either ws:// or wss://.
  /// </param>
  static std::unique_ptr<IWebSocket> Make(const std::string& url);

  // Only use if a legacy implementation is required as fallback.
  static std::unique_ptr<IWebSocket> MakeLegacy(const std::string& url);

  virtual ~IWebSocket() {}

  /// <summary>
  /// Establishes a continuous connection with the remote endpoint.
  /// </summary>
  /// <param name="protocols">
  /// Currently unused
  /// </param>
  /// <param name="options">
  /// HTTP header fields passed by the remote endpoint, to be used in the handshake process.
  /// </param>
  virtual void Connect(const Protocols& protocols = {}, const Options& options = {}) = 0;

  /// <summary>
  /// Sends a ping frame to the remote endpoint.
  /// </summary>
  virtual void Ping() = 0;

  /// <summary>
  /// Sends a text message to the remote endpoint.
  /// </summary>
  /// <param name="message">
  /// UTF8-encoded string of arbitrary length.
  /// </param>
  virtual void Send(const std::string& message) = 0;

  /// <summary>
  /// Sends a non-plain-text message to the remote endpoint.
  /// </summary>
  /// <param name="base64String">
  /// Binary message encoded in Base64 format.
  /// </param>
  virtual void SendBinary(const std::string& base64String) = 0;

  /// <summary>
  /// Terminates this resource's connection to the remote endpoint.
  /// This instance can't be restarted or re-connected afterwards.
  /// </summary>
  /// <param name="close">
  /// </param>
  /// <param name="reason">
  /// </param>
  virtual void Close(CloseCode code, const std::string& reason) = 0;

  /// <returns>
  /// Current public state as defined in the <c>ReadyState</c> enum.
  /// </returns>
  virtual ReadyState GetReadyState() const = 0;

  /// <summary>
  /// Sets the optional custom behavior on a successful connection.
  /// </summary>
  /// <param name="handler">
  /// </param>
  virtual void SetOnConnect(std::function<void()>&& handler) = 0;

  /// <summary>
  /// Sets the optional custom behavior on a successful ping to the remote endpoint.
  /// </summary>
  /// <param name="handler">
  /// </param>
  virtual void SetOnPing(std::function<void()>&& handler) = 0;

  /// <summary>
  /// Sets the optional custom behavior on a message sending.
  /// </summary>
  /// <param name="handler">
  /// </param>
  virtual void SetOnSend(std::function<void(std::size_t)>&& handler) = 0;

  /// <summary>
  /// Sets the optional custom behavior to run when there is an incoming message.
  /// </summary>
  /// <param name="handler">
  /// </param>
  virtual void SetOnMessage(std::function<void(std::size_t, const std::string&)>&& handler) = 0;

  /// <summary>
  /// Sets the optional custom behavior to run when this instance is closed.
  /// </summary>
  /// <param name="handler">
  /// </param>
  virtual void SetOnClose(std::function<void(CloseCode, const std::string&)>&& handler) = 0;

  /// <summary>
  /// Sets the optional custom behavior on an error condition.
  /// </summary>
  /// <param name="handler">
  /// </param>
  virtual void SetOnError(std::function<void(Error&&)>&& handler) = 0;
};

} // namespace Microsoft::React

// Deprecated. Keeping for compatibility with dependent code.
namespace facebook::react {

using IWebSocket = Microsoft::React::IWebSocket;

} // namespace facebook::react
