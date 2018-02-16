using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using ReactNative.Common;
using ReactNative.Modules.Core;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using WebSocketSharp;

namespace ReactNative.Modules.WebSocket
{
    class WebSocketModule : ReactContextNativeModuleBase
    {
        private readonly IDictionary<int, WebSocketSharp.WebSocket> _webSocketConnections = new Dictionary<int, WebSocketSharp.WebSocket>();

        private readonly object locker = new object();

        #region Constructor(s)

        public WebSocketModule(ReactContext reactContext)
            : base(reactContext)
        {
        }

        #endregion

        #region NativeModuleBase Overrides

        public override string Name => "WebSocketModule";

        #endregion

        #region Public Methods

        [ReactMethod]
        public void connect(string url, string[] protocols, JObject options, int id)
        {
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

            InitializeInBackground(url, webSocket, options);
        }

        [ReactMethod]
        public void close(ushort code, string reason, int id)
        {
            WebSocketSharp.WebSocket webSocket;

            lock (this.locker)
            {
                if (!_webSocketConnections.TryGetValue(id, out webSocket))
                {
                    Tracer.Write(
                        ReactConstants.Tag,
                        $"Cannot close WebSocket. Unknown WebSocket id {id}.");

                    return;
                }
            }

            try
            {
                webSocket.Close(code, reason);
            }
            catch (Exception ex)
            {
                lock (this.locker)
                {
                    if (_webSocketConnections.ContainsKey(id))
                    {
                        _webSocketConnections.Remove(id);
                    }

                    Tracer.Error(
                        ReactConstants.Tag,
                        $"Could not close WebSocket connection for id '{id}'.",
                        ex);
                }
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
                lock (this.locker)
                {
                    _webSocketConnections.Add(id, webSocket);

                    SendEvent("websocketOpen", new JObject
                    {
                        {"id", id},
                    });
                }
            }
        }

        private void OnClosed(int id, object webSocket, CloseEventArgs args)
        {
            lock (this.locker)
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
        }

        private void OnError(int id, WebSocketSharp.ErrorEventArgs args)
        {
            lock (this.locker)
            {
                if (_webSocketConnections.ContainsKey(id))
                {
                    _webSocketConnections.Remove(id);
                }
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

        private void InitializeInBackground(string url, WebSocketSharp.WebSocket webSocket, JObject options)
        {
            var parsedOptions = new WebSocketOptions(options);
            ProxyHelper proxy = null;

            if (parsedOptions.UseDefaultProxy)
            {
                proxy = new ProxyHelper(new Uri(url));
            }
            else
            {
                proxy = new ProxyHelper(parsedOptions);
            }

            if (proxy != null && !proxy.IsBypassed)
            {
                webSocket.SetProxy(proxy.ProxyAddress, proxy.UserName, proxy.Password);
            }

            webSocket.Origin = parsedOptions.Origin;

            webSocket.Connect();
        }

        private void SendMessageInBackground(int id, string message)
        {
            WebSocketSharp.WebSocket webSocket;

            lock (this.locker)
            {
                if (!_webSocketConnections.TryGetValue(id, out webSocket))
                {
                    SendEvent("websocketFailed", new JObject
                    {
                        { "id", id },
                        { "message", $"Unknown WebSocket id {id}." },
                    });
                }
            }

            webSocket?.SendAsync(message, null);
        }

        private void SendMessageInBackground(int id, byte[] message)
        {
            WebSocketSharp.WebSocket webSocket;

            lock (this.locker)
            {
                if (!_webSocketConnections.TryGetValue(id, out webSocket))
                {
                    SendEvent("websocketFailed", new JObject
                    {
                        { "id", id },
                        { "message", $"Unknown WebSocket id {id}." },
                    });
                }
            }

            webSocket?.SendAsync(message, null);
        }

        private void SendEvent(string eventName, JObject parameters)
        {
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit(eventName, parameters);
        }

        #endregion
    }
}
