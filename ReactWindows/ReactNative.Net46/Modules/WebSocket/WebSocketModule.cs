﻿using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using ReactNative.Common;
using ReactNative.Modules.Core;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using WebSocketSharp;
using static System.FormattableString;

namespace ReactNative.Modules.WebSocket
{
    class WebSocketModule : ReactContextNativeModuleBase
    {
        private readonly IDictionary<int, WebSocketSharp.WebSocket> _webSocketConnections = new Dictionary<int, WebSocketSharp.WebSocket>();

        #region Constructor(s)

        public WebSocketModule(ReactContext reactContext)
            : base(reactContext)
        {
        }

        #endregion

        #region NativeModuleBase Overrides

        public override string Name
        {
            get
            {
                return "WebSocketModule";
            }
        }

        #endregion

        #region Public Methods

        [ReactMethod]
        public void connect(string url, string[] protocols, JObject options, int id)
        {
            if (options != null && options.ContainsKey("origin"))
            {
                throw new NotImplementedException(/* TODO: (#253) */);
            }

            var webSocket = new WebSocketSharp.WebSocket(url);

            webSocket.OnMessage += (sender, args) =>
            {
                OnMessageReceived(id, sender, args);
            };

            webSocket.OnOpen += (sender, args) =>
            {
                OnOpen(id, webSocket, args);
            };

            webSocket.OnError += (sender, args) =>
            {
                OnError(id, args);
            };

            webSocket.OnClose += (sender, args) =>
            {
                OnClosed(id, sender, args);
            };

            InitializeInBackground(id, url, webSocket);
        }

        [ReactMethod]
        public void close(ushort code, string reason, int id)
        {
            WebSocketSharp.WebSocket webSocket;

            if (!_webSocketConnections.TryGetValue(id, out webSocket))
            {
                Tracer.Write(
                    ReactConstants.Tag,
                    Invariant($"Cannot close WebSocket. Unknown WebSocket id {id}."));

                return;
            }

            try
            {
                webSocket.Close(code, reason);
            }
            catch (Exception ex)
            {
                if (_webSocketConnections.ContainsKey(id))
                {
                    _webSocketConnections.Remove(id);
                }

                Tracer.Error(
                    ReactConstants.Tag,
                    Invariant($"Could not close WebSocket connection for id '{id}'."),
                    ex);
            }
        }

        [ReactMethod]
        public void send(string message, int id)
        {
            SendMessageInBackground(id, message);
        }

        #endregion

        #region Event Handlers

        private void OnOpen(int id, WebSocketSharp.WebSocket webSocket, EventArgs args)
        {
            if (webSocket != null)
            {
                _webSocketConnections.Add(id, webSocket);

                SendEvent("websocketOpen", new JObject
                {
                    {"id", id},
                });
            }
        }

        private void OnClosed(int id, object webSocket, CloseEventArgs args)
        {
            if (_webSocketConnections.ContainsKey(id))
            {
                _webSocketConnections.Remove(id);

                SendEvent("websocketClosed", new JObject
                {
                    {"id", id},
                    {"code", args.Code},
                    {"reason", args.Reason},
                });
            }
            else
            {
                SendEvent("websocketFailed", new JObject
                {
                    { "id", id },
                    {"code", args.Code},
                    { "message", args.Reason },
                });
            }
        }

        private void OnError(int id, WebSocketSharp.ErrorEventArgs args)
        {
            if (_webSocketConnections.ContainsKey(id))
            {
                _webSocketConnections.Remove(id);
            }

            SendEvent("websocketFailed", new JObject
            {
                { "id", id },
                { "message", args.Message },
            });
        }

        private void OnMessageReceived(int id, object sender, MessageEventArgs args)
        {
            var message = args.Data;
            SendEvent("websocketMessage", new JObject
                {
                    { "id", id },
                    { "data", message },
                });
        }

        #endregion

        #region Private Methods

        private void InitializeInBackground(int id, string url, WebSocketSharp.WebSocket webSocket)
        {
            webSocket?.Connect();
        }

        private void SendMessageInBackground(int id, string message)
        {
            _webSocketConnections[id].SendAsync(message, null);
        }

        private void SendEvent(string eventName, JObject parameters)
        {
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit(eventName, parameters);
        }

        #endregion
    }
}
