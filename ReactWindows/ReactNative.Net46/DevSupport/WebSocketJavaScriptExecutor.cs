// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using ReactNative.Modules.DevSupport;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.DevSupport
{
    class WebSocketJavaScriptExecutor : IJavaScriptExecutor
    {
        private readonly IDeveloperSettings _developerSettings;
        private const int ConnectTimeoutMilliseconds = 5000;
        private const int ConnectRetryCount = 3;
        private const int PrepareTimeoutMilliseconds = 2000;
        private const int PrepareRetryCount = 3;

        private WebSocketSharp.WebSocket _webSocket;
        private readonly JObject _injectedObjects;
        private readonly IDictionary<int, TaskCompletionSource<JToken>> _callbacks;

        private HttpListener _nativeHookListener;
        private CallSerializableNativeHook _callSerializableNativeHook;

        private int _requestId;
        private bool _isDisposed;

        public WebSocketJavaScriptExecutor(IDeveloperSettings developerSettings)
        {
            _developerSettings = developerSettings;
            _injectedObjects = new JObject();
            _callbacks = new ConcurrentDictionary<int, TaskCompletionSource<JToken>>();
            StartNativeHookListener();
        }

        public async Task ConnectAsync(string webSocketServerUrl, CancellationToken token)
        {
            _webSocket = await ConnectCoreTaskAsync(webSocketServerUrl, token);
            await PrepareJavaScriptRuntimeAsync(token);
        }

        private async Task PrepareJavaScriptRuntimeAsync(CancellationToken token)
        {
            var retryCount = PrepareRetryCount;
            while (true)
            {
                using (var timeoutSource = new CancellationTokenSource(PrepareTimeoutMilliseconds))
                using (var linkedTokenSource = CancellationTokenSource.CreateLinkedTokenSource(timeoutSource.Token, token))
                {
                    try
                    {
                        await PrepareJavaScriptRuntimeTaskAsync(linkedTokenSource.Token);
                        return;
                    }
                    catch (OperationCanceledException ex)
                    when (ex.CancellationToken == timeoutSource.Token)
                    {
                        token.ThrowIfCancellationRequested();
                    }
                    catch
                    {
                        if (--retryCount <= 0)
                        {
                            throw;
                        }
                    }
                }
            }
        }

        private async Task<WebSocketSharp.WebSocket> ConnectCoreTaskAsync(string webSocketServerUrl, CancellationToken token)
        {
            var retryCount = ConnectRetryCount;

            var webSocket = new WebSocketSharp.WebSocket(webSocketServerUrl);
            var callback = new TaskCompletionSource<JToken>();

            webSocket.OnMessage += (sender, args) =>
            {
                if (args.IsText)
                {
                    OnMessageReceived(args.Data);
                }
            };

            webSocket.OnOpen += (sender, args) =>
            {
                callback.SetResult(true);
            };

            webSocket.OnError += (sender, args) =>
            {
                OnClosed();
            };

            webSocket.OnClose += (sender, args) =>
            {
                OnClosed();
            };

            while (true)
            {
                var cancellationSource = new TaskCompletionSource<bool>();
                using (var timeoutSource = new CancellationTokenSource(ConnectTimeoutMilliseconds))
                using (var linkedTokenSource = CancellationTokenSource.CreateLinkedTokenSource(timeoutSource.Token, token))
                using (linkedTokenSource.Token.Register(() => cancellationSource.SetResult(false)))
                {
                    try
                    {
                        webSocket.ConnectAsync();
                        await Task.WhenAny(callback.Task, cancellationSource.Task);
                        timeoutSource.Token.ThrowIfCancellationRequested();
                        token.ThrowIfCancellationRequested();

                        return webSocket;
                    }
                    catch (OperationCanceledException ex)
                    when (ex.CancellationToken == timeoutSource.Token)
                    {
                        token.ThrowIfCancellationRequested();
                    }
                    catch
                    {
                        if (--retryCount <= 0)
                        {
                            throw;
                        }
                    }
                }
            }
        }

        public JToken CallFunctionReturnFlushedQueue(string moduleName, string methodName, JArray arguments)
        {
            return Call("callFunctionReturnFlushedQueue", new JArray
            {
                moduleName,
                methodName,
                arguments,
            });
        }

        public JToken InvokeCallbackAndReturnFlushedQueue(int callbackId, JArray arguments)
        {
            return Call("invokeCallbackAndReturnFlushedQueue", new JArray
            {
                callbackId,
                arguments,
            });
        }

        public JToken FlushedQueue()
        {
            return Call("flushedQueue", new JArray());
        }

        public void RunScript(string script, string sourceUrl)
        {
            var requestId = Interlocked.Increment(ref _requestId);
            var callback = new TaskCompletionSource<JToken>();
            _callbacks.Add(requestId, callback);

            try
            {
                var request = new JObject
                {
                    { "id", requestId },
                    { "method", "executeApplicationScript" },
                    { "url", script },
                    { "inject", _injectedObjects },
                };

                SendMessageAsync(requestId, request.ToString(Formatting.None));
                callback.Task.Wait();
            }
            catch (AggregateException ex)
            when (ex.InnerExceptions.Count == 1)
            {
                throw ex.InnerException;
            }
            finally
            {
                _callbacks.Remove(requestId);
            }
        }

        public void SetGlobalVariable(string propertyName, JToken value)
        {
            _injectedObjects.Add(propertyName, value.ToString(Formatting.None));
        }

        public void SetCallSerializableNativeHook(CallSerializableNativeHook callSerializableNativeHook)
        {
            _callSerializableNativeHook = callSerializableNativeHook;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (_isDisposed)
            {
                return;
            }

            if (disposing)
            {
                if (_webSocket != null)
                {
                    if (_webSocket.IsAlive)
                    {
                        _webSocket.CloseAsync();
                    }

                    _webSocket = null;
                }

                if (_nativeHookListener != null)
                {
                    if (_nativeHookListener.IsListening)
                    {
                        _nativeHookListener.Close();
                    }

                    _nativeHookListener = null;
                }
            }

            _isDisposed = true;
        }

        private JToken Call(string methodName, JArray arguments)
        {
            var requestId = Interlocked.Increment(ref _requestId);
            var callback = new TaskCompletionSource<JToken>();
            _callbacks.Add(requestId, callback);

            try
            {
                var request = new JObject
                {
                    { "id", requestId },
                    { "method", methodName },
                    { "arguments", arguments },
                };

                SendMessageAsync(requestId, request.ToString(Formatting.None));
                return callback.Task.Result;
            }
            catch (AggregateException ex)
            when (ex.InnerExceptions.Count == 1)
            {
                ExceptionDispatchInfo.Capture(ex.InnerException).Throw();
                /* Should not */
                throw;
            }
            finally
            {
                _callbacks.Remove(requestId);
            }
        }

        private async Task<JToken> PrepareJavaScriptRuntimeTaskAsync(CancellationToken token)
        {
            var cancellationSource = new TaskCompletionSource<bool>();
            using (token.Register(() => cancellationSource.SetResult(false)))
            {
                var requestId = Interlocked.Increment(ref _requestId);
                var callback = new TaskCompletionSource<JToken>();
                _callbacks.Add(requestId, callback);

                try
                {
                    var request = new JObject
                    {
                        { "id", requestId },
                        { "method", "prepareJSRuntime" },
                    };

                    SendMessageAsync(requestId, request.ToString(Formatting.None));
                    await Task.WhenAny(callback.Task, cancellationSource.Task);
                    token.ThrowIfCancellationRequested();
                    return await callback.Task;
                }
                finally
                {
                    _callbacks.Remove(requestId);
                }
            }
        }

        private void SendMessageAsync(int requestId, string message)
        {
            if (IsAlive)
            {
                _webSocket.SendAsync(message, null);
            }
            else
            {
                var callback = default(TaskCompletionSource<JToken>);
                if (_callbacks.TryGetValue(requestId, out callback))
                {
                    callback.TrySetResult(new JValue((Uri)null)); // Creating a JValue with JTokenType.Null
                }
            }
        }

        private void OnMessageReceived(string message)
        {
            var json = JObject.Parse(message);
            if (json.ContainsKey("replyID"))
            {
                var replyId = json.Value<int>("replyID");
                var callback = default(TaskCompletionSource<JToken>);
                if (_callbacks.TryGetValue(replyId, out callback))
                {
                    var result = default(JToken);
                    if (json != null && json.TryGetValue("result", out result))
                    {
                        if (result.Type == JTokenType.String)
                        {
                            callback.TrySetResult(JToken.Parse(result.Value<string>()));
                        }
                        else
                        {
                            callback.TrySetResult(result);
                        }
                    }
                    else
                    {
                        callback.TrySetResult(null);
                    }
                }
            }
        }

        private void StartNativeHookListener()
        {
            IPAddress devAddr;
            try
            {
                devAddr = _developerSettings.DeviceDebugIpAddress;
            }
            catch
            {
                Debug.WriteLine("Failed to get dev port IP address.");
                return;
            }

            var port = getFreeTcpPort();
            var url = $"http://{devAddr}:{port}/";
            _nativeHookListener = new HttpListener();
            _nativeHookListener.Prefixes.Add(url);
            _nativeHookListener.Start();

            Task.Factory.StartNew(
                async () =>
                {
                    var listener = _nativeHookListener;
                    while (listener.IsListening)
                    {
                        await Listen(listener);
                    }
                },
                TaskCreationOptions.LongRunning);

            Debug.WriteLine($"__NATIVE_HOOK_URL__={url}");
            var nativeHookUrl = new JValue(url);
            _injectedObjects.Add("__NATIVE_HOOK_URL__", nativeHookUrl.ToString(Formatting.None));
        }

        private static int getFreeTcpPort()
        {
            var l = new TcpListener(IPAddress.Loopback, 0);
            l.Start();
            int port = ((IPEndPoint) l.LocalEndpoint).Port;
            l.Stop();
            return port;
        }

        private async Task Listen(HttpListener listener)
        {
            try
            {
                var context = await listener.GetContextAsync();
                HandleRequest(context);
            }
            catch (HttpListenerException)
            {
            }
        }

        private void HandleRequest(HttpListenerContext context)
        {
            var response = context.Response;
            response.StatusCode = 200;
            response.Headers.Add("Access-Control-Allow-Origin", "*");

            string result = "{}";
            try
            {
                JObject request;
                using (var reader = new StreamReader(context.Request.InputStream, context.Request.ContentEncoding))
                {
                    request = JObject.Parse(reader.ReadToEnd());
                }

                if (_callSerializableNativeHook != null && ((string)request["type"]).Equals("NativeCallSyncHook"))
                {
                    JArray args = (JArray)request["args"];
                    result = _callSerializableNativeHook(
                        (int)args[0],
                        (int)args[1],
                        (JArray)args[2]).ToString(Formatting.None);
                }
                else
                {
                }
            }
            catch
            {
            }

            byte[] content = Encoding.UTF8.GetBytes(result);
            response.OutputStream.Write(content, 0, content.Length);
            response.Close();
        }

        private void OnClosed()
        {
            Dispose();
        }

        private bool IsAlive
        {
            get
            {
                return !_isDisposed && _webSocket != null && _webSocket.IsAlive;
            }
        }
    }
}
