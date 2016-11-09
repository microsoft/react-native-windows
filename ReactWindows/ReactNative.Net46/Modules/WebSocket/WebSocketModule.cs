using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using ReactNative.Common;
using ReactNative.Modules.Core;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.WebSockets;
using System.Threading;
using WebSocketSharp;
using static System.FormattableString;

namespace ReactNative.Modules.WebSocket
{
    class WebSocketModule : ReactContextNativeModuleBase
    {
        private readonly IDictionary<int, WebSocketSharp.WebSocket> _webSocketConnections;
        private readonly IDictionary<int, StringWriter> _dataWriters;

        public WebSocketModule(ReactContext reactContext)
            : base(reactContext)
        {
            _webSocketConnections = new Dictionary<int, WebSocketSharp.WebSocket>();
            _dataWriters = new Dictionary<int, StringWriter>();
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
            var webSocket = new WebSocketSharp.WebSocket(url);

            //webSocket.Control.MessageType = SocketMessageType.Utf8;

            //if (protocols != null)
            //{
            //    foreach (var protocol in protocols)
            //    {
            //        webSocket.Control.SupportedProtocols.Add(protocol);
            //    }
            //}

            if (options != null && options.ContainsKey("origin"))
            {
                throw new NotImplementedException(/* TODO: (#253) */);
            }

            webSocket.OnMessage += (sender, args) =>
            {
                OnMessageReceived(id, sender, args);
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
            var webSocket = default(WebSocketSharp.WebSocket);
            if (!_webSocketConnections.TryGetValue(id, out webSocket))
            {
                Tracer.Write(
                    ReactConstants.Tag,
                    Invariant($"Cannot close WebSocket. Unknown WebSocket id {id}."));

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
                Tracer.Error(
                    ReactConstants.Tag,
                    Invariant($"Could not close WebSocket connection for id '{id}'."),
                    ex);
            }
        }

        [ReactMethod]
        public void send(string message, int id)
        {
            //var dataWriter = default(DataWriter);
            //if (!_dataWriters.TryGetValue(id, out dataWriter))
            //{
            //    throw new InvalidOperationException(
            //        Invariant($"Cannot send a message. Unknown WebSocket id '{id}'."));
            //}

            SendMessageInBackground(id, message);
        }

        private void InitializeInBackground(int id, string url, WebSocketSharp.WebSocket webSocket)
        {
            try
            {
                webSocket.Connect();
                _webSocketConnections.Add(id, webSocket);

                //var dataWriter = new StringWriter(webSocket.OutputStream);
                //dataWriter.UnicodeEncoding = UnicodeEncoding.Utf8;
                //_dataWriters.Add(id, dataWriter);

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

        private void SendMessageInBackground(int id, string message)
        {
            try
            {
                //dataWriter.WriteString(message);
                //await dataWriter.StoreAsync().AsTask().ConfigureAwait(false);
                _webSocketConnections[id].SendAsync(message, null);
            }
            catch (Exception ex)
            {
                OnError(id, ex);
            }
        }

        private void OnClosed(int id, object webSocket, CloseEventArgs args)
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

        private void OnMessageReceived(int id, object sender, MessageEventArgs args)
        {
            try
            {
                var message = args.Data;
                SendEvent("websocketMessage", new JObject
                    {
                        { "id", id },
                        { "data", message },
                    });

                //using (var reader = args.GetDataReader())
                //{
                //    var message = reader.ReadString(reader.UnconsumedBufferLength);
                //SendEvent("websocketMessage", new JObject
                //    {
                //        { "id", id },
                //        { "data", message },
                //    });
                //}
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
