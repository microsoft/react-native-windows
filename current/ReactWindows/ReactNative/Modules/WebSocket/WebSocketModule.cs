// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Json;
using ReactNative.Modules.Core;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using static System.FormattableString;

namespace ReactNative.Modules.WebSocket
{
    class WebSocketModule : ReactContextNativeModuleBase
    {
        private readonly IDictionary<int, MessageWebSocket> _webSocketConnections;
        private readonly IDictionary<int, DataWriter> _dataWriters;

        public WebSocketModule(ReactContext reactContext)
            : base(reactContext)
        {
            _webSocketConnections = new Dictionary<int, MessageWebSocket>();
            _dataWriters = new Dictionary<int, DataWriter>();
        }

        public override string Name
        {
            get
            {
                return "WebSocketModule";
            }
        }

        [ReactMethod]
        public void connect(string url, string[] protocols, JObject options, int id)
        {
            var webSocket = new MessageWebSocket();

            if (protocols != null)
            {
                foreach (var protocol in protocols)
                {
                    webSocket.Control.SupportedProtocols.Add(protocol);
                }
            }

            if (options != null && options.ContainsKey("origin"))
            {
                throw new NotImplementedException(/* TODO: (#253) */);
            }

            webSocket.MessageReceived += (sender, args) =>
            {
                OnMessageReceived(id, sender, args);
            };

            webSocket.Closed += (sender, args) =>
            {
                OnClosed(id, sender, args);
            };

            InitializeInBackground(id, url, webSocket);
        }

        [ReactMethod]
        public void close(ushort code, string reason, int id)
        {
            if (!_webSocketConnections.TryGetValue(id, out var webSocket))
            {
                RnLog.Warn(ReactConstants.RNW, $"Cannot close WebSocket. Unknown WebSocket id {id}.");

                return;
            }

            try
            {
                var writer = _dataWriters[id];
                _dataWriters.Remove(id);
                _webSocketConnections.Remove(id);
                writer.Dispose();
                webSocket.Close(code, reason);
            }
            catch (Exception ex)
            {
                RnLog.Error(ReactConstants.RNW, ex, $"Could not close WebSocket connection for id '{id}'.");
            }
        }

        [ReactMethod]
        public void send(string message, int id)
        {
            if (!_webSocketConnections.TryGetValue(id, out var webSocket))
            {
                throw new InvalidOperationException(
                    Invariant($"Cannot send a message. Unknown WebSocket id '{id}'."));
            }

            webSocket.Control.MessageType = SocketMessageType.Utf8;

            if (!_dataWriters.TryGetValue(id, out var dataWriter))
            {
                throw new InvalidOperationException(
                    Invariant($"Cannot send a message. Unknown WebSocket id '{id}'."));
            }

            SendMessageInBackground(id, dataWriter, message);
        }

        [ReactMethod]
        public void sendBinary(string message, int id)
        {
            if (!_webSocketConnections.TryGetValue(id, out var webSocket))
            {
                throw new InvalidOperationException(
                    Invariant($"Cannot send a message. Unknown WebSocket id '{id}'."));
            }

            webSocket.Control.MessageType = SocketMessageType.Binary;

            if (!_dataWriters.TryGetValue(id, out var dataWriter))
            {
                throw new InvalidOperationException(
                    Invariant($"Cannot send a message. Unknown WebSocket id '{id}'."));
            }

            SendMessageInBackground(id, dataWriter, Convert.FromBase64String(message));
        }

        private async void InitializeInBackground(int id, string url, MessageWebSocket webSocket)
        {
            try
            {
                await webSocket.ConnectAsync(new Uri(url)).AsTask().ConfigureAwait(false);
                _webSocketConnections.Add(id, webSocket);

                var dataWriter = new DataWriter(webSocket.OutputStream)
                {
                    UnicodeEncoding = UnicodeEncoding.Utf8,
                };

                _dataWriters.Add(id, dataWriter);

                SendEvent("websocketOpen", new JObject
                {
                    { "id", id },
                });
            }
            catch (Exception ex)
            {
                OnError(id, ex);
            }
        }

        private async void SendMessageInBackground(int id, DataWriter dataWriter, string message)
        {
            try
            {
                dataWriter.WriteString(message);
                await dataWriter.StoreAsync().AsTask().ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                OnError(id, ex);
            }
        }

        private async void SendMessageInBackground(int id, DataWriter dataWriter, byte[] message)
        {
            try
            {
                dataWriter.WriteBytes(message);
                await dataWriter.StoreAsync().AsTask().ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                OnError(id, ex);
            }
        }

        private void OnClosed(int id, IWebSocket webSocket, WebSocketClosedEventArgs args)
        {
            SendEvent("websocketClosed", new JObject
            {
                { "id", id },
                { "code", args.Code },
                { "reason", args.Reason },
            });
        }

        private void OnError(int id, Exception exception)
        {
            SendEvent("websocketFailed", new JObject
            {
                { "id", id },
                { "message", exception.Message },
            });
        }

        private void OnMessageReceived(int id, MessageWebSocket sender, MessageWebSocketMessageReceivedEventArgs args)
        {
            try
            {
                using (var reader = args.GetDataReader())
                {
                    var message = args.MessageType == SocketMessageType.Binary
                        ? Convert.ToBase64String(reader.ReadBuffer(reader.UnconsumedBufferLength).ToArray())
                        : reader.ReadString(reader.UnconsumedBufferLength);

                    SendEvent("websocketMessage", new JObject
                    {
                        { "id", id },
                        { "data", message },
                        { "type", args.MessageType == SocketMessageType.Binary ? "binary" : "text" },
                    });
                }
            }
            catch (Exception ex)
            {
                OnError(id, ex);
            }
        }

        private void SendEvent(string eventName, JObject parameters)
        {
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit(eventName, parameters);
        }
    }
}
