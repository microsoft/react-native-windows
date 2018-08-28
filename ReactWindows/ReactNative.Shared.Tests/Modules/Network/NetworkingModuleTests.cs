// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.Modules.Network;
using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Web.Http;
#else
using System.Net;
using System.Net.Http;
#endif

namespace ReactNative.Tests.Modules.Network
{
    [TestFixture]
    public class NetworkingModuleTests
    {
        [Test]
        public void NetworkingModule_ArgumentChecks()
        {
            var module = CreateNetworkingModule(new DefaultHttpClient(), new MockInvocationHandler());

            AssertEx.Throws<ArgumentNullException>(
                () => module.sendRequest(null, new Uri("http://example.com"), 0, null, null, "text", false, 0),
                ex => Assert.AreEqual("method", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => module.sendRequest("get", null, 0, null, null, "text", false, 0),
                ex => Assert.AreEqual("url", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => module.sendRequest("get", new Uri("http://example.com"), 0, null, null, null, false, 0),
                ex => Assert.AreEqual("responseType", ex.ParamName));

            AssertEx.Throws<ArgumentOutOfRangeException>(
                () => module.sendRequest("get", new Uri("http://example.com"), 0, null, null, "", false, 0),
                ex => Assert.AreEqual("responseType", ex.ParamName));
        }

        [Test]
        public void NetworkingModule_Request_Method()
        {
            var method = "GET";

            var passed = false;
            var waitHandle = new AutoResetEvent(false);
            var httpClient = new MockHttpClient(request =>
            {
                passed = request.Method.ToString() == method;
                waitHandle.Set();
                return new HttpResponseMessage(HttpStatusCode.NoContent);
            });

            var module = CreateNetworkingModule(httpClient, new MockInvocationHandler());
            module.sendRequest(method, new Uri("http://example.com"), 1, null, null, "text", false, 1000);
            waitHandle.WaitOne();
            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Request_NoContent_Null()
        {
            var method = "GET";

            var passed = false;
            var waitHandle = new AutoResetEvent(false);
            var httpClient = new MockHttpClient(request =>
            {
                passed = request.Method.ToString() == method;
                waitHandle.Set();
                return new HttpResponseMessage(HttpStatusCode.NoContent);
            });

            var module = CreateNetworkingModule(httpClient, new MockInvocationHandler());
            module.sendRequest(method, new Uri("http://example.com"), 1, null, null, "text", false, 1000);
            waitHandle.WaitOne();
            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Request_NoContent_NonNull()
        {
            var method = "GET";

            var passed = false;
            var waitHandle = new AutoResetEvent(false);
            var httpClient = new MockHttpClient(request =>
            {
                passed = request.Method.ToString() == method;
                waitHandle.Set();
                return new HttpResponseMessage(HttpStatusCode.NoContent);
            });

            var module = CreateNetworkingModule(httpClient, new MockInvocationHandler());
            module.sendRequest(method, new Uri("http://example.com"), 1, null, new JObject(), "text", false, 1000);
            waitHandle.WaitOne();
            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Request_Headers()
        {
            var headers = new[]
            {
                new[] { "foo", "bar" },
                new[] { "X-RNTest", "42.424242" },
            };

            var passed = true;
            var waitHandle = new AutoResetEvent(false);
            var httpClient = new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                passed &= request.Headers["foo"] == "bar";
                passed &= request.Headers["X-RNTest"] == "42.424242";
#else
                passed &= request.Headers.GetValues("foo").First() == "bar";
                passed &= request.Headers.GetValues("X-RNTest").First() == "42.424242";
#endif

                waitHandle.Set();
                return new HttpResponseMessage(HttpStatusCode.NoContent);
            });

            var module = CreateNetworkingModule(httpClient, new MockInvocationHandler());

            module.sendRequest("get", new Uri("http://example.com"), 1, headers, null, "text", false, 1000);
            waitHandle.WaitOne();
            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Request_Content_String()
        {
            var data = new JObject
            {
                { "string", "Hello World" },
            };

            var headers = new[]
            {
                new[] { "Content-Type", "text/plain" },
            };

            var passed = true;
            var waitHandle = new AutoResetEvent(false);
            var module = CreateNetworkingModule(new MockHttpClient(request =>
                {
#if WINDOWS_UWP
                    var body = request.Content.ReadAsStringAsync().AsTask().Result;
#else
                    var body = request.Content.ReadAsStringAsync().Result;
#endif
                    var mediaType = request.Content.Headers.ContentType.ToString();
                    passed &= body == "Hello World";
                    passed &= mediaType == "text/plain";

                    waitHandle.Set();
                    return null;
                }),
                new MockInvocationHandler());

            module.sendRequest("post", new Uri("http://example.com"), 1, headers, data, "text", false, 1000);
            waitHandle.WaitOne();

            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Request_Content_Base64()
        {
            var data = new JObject
            {
                { "base64", Convert.ToBase64String(Encoding.UTF8.GetBytes("Hello World")) },
            };

            var headers = new[]
            {
                new[] { "Content-Type", "text/plain" },
            };

            var passed = true;
            var waitHandle = new AutoResetEvent(false);
            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                var body = request.Content.ReadAsStringAsync().AsTask().Result;
#else
                var body = request.Content.ReadAsStringAsync().Result;
#endif
                var mediaType = request.Content.Headers.ContentType.ToString();
                passed &= body == "Hello World";
                passed &= mediaType == "text/plain";

                waitHandle.Set();
                return null;
            }),
                new MockInvocationHandler());

            module.sendRequest("post", new Uri("http://example.com"), 1, headers, data, "text", false, 1000);
            waitHandle.WaitOne();

            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Request_Content_String_NoContentType()
        {
            var data = new JObject
            {
                { "string", "Hello World" },
            };

            var passed = true;
            var waitHandle = new AutoResetEvent(false);
            var module = CreateNetworkingModule(new DefaultHttpClient(), new MockInvocationHandler((name, args) =>
            {
                if (name != "emit" || args.Length != 2 || ((string)args[0]) != "didCompleteNetworkResponse")
                {
                    return;
                }

                var array = args[1] as JArray;
                if (array == null || array.Count != 3)
                {
                    return;
                }

                passed = true;
                waitHandle.Set();
            }));

            module.sendRequest("post", new Uri("http://example.com"), 1, null, data, "text", false, 1000);
            waitHandle.WaitOne();

            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Request_Content_String_Gzip()
        {
            var data = new JObject
            {
                { "string", "Hello World" },
            };

            var headers = new[]
            {
                new[] { "Content-Type", "text/plain" },
                new[] { "Content-Encoding", "gzip" },
            };

            var passed = true;
            var waitHandle = new AutoResetEvent(false);
            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                var inputStream = request.Content.ReadAsInputStreamAsync()
                    .AsTask()
                    .Result
                    .AsStreamForRead();
#else
                var inputStream = request.Content.ReadAsStreamAsync().Result;
#endif
                var gzipStream = new GZipStream(
                    inputStream,
                    CompressionMode.Decompress);

                using (var reader = new StreamReader(gzipStream))
                {
                    var body = reader.ReadToEnd();
                    var mediaType = request.Content.Headers.ContentType.ToString();
                    var encoding = request.Content.Headers.ContentEncoding.ToString();
                    passed &= body == "Hello World";
                    passed &= mediaType == "text/plain";
                    passed &= encoding == "gzip";

                    waitHandle.Set();
                    return null;
                }
            }),
            new MockInvocationHandler());

            module.sendRequest("post", new Uri("http://example.com"), 1, headers, data, "text", false, 1000);
            waitHandle.WaitOne();

            Assert.IsTrue(passed);
        }

        [Test]
        public void NetworkingModule_Response_Headers()
        {
            var onReceived = new AutoResetEvent(false);
            var onComplete = new AutoResetEvent(false);
            var onReceivedData = default(JArray);
            var onCompleteData = default(JArray);
            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
                var response = new HttpResponseMessage(HttpStatusCode.NoContent);
                response.RequestMessage = request;
#if WINDOWS_UWP
                response.Headers["X-Foo"] = "bar";
#else
                response.Headers.Add("X-Foo", "bar");
#endif
                return response;
            }),
            new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = args[0] as string;
                    if (eventName == "didReceiveNetworkResponse")
                    {
                        onReceivedData = args[1] as JArray;
                        onReceived.Set();
                    }
                    else if (eventName == "didCompleteNetworkResponse")
                    {
                        onCompleteData = args[1] as JArray;
                        onComplete.Set();
                    }
                }
            }));

            var uri = new Uri("http://example.com");
            module.sendRequest("get", uri, 42, null, null, "text", false, 1000);

            onReceived.WaitOne();
            Assert.IsNotNull(onReceivedData);
            Assert.AreEqual(42, onReceivedData[0].Value<int>());
            Assert.AreEqual(204, onReceivedData[1].Value<int>());

            var headerData = onReceivedData[2].Value<JObject>();
            Assert.AreEqual("bar", headerData.Value<string>("X-Foo"));

            Assert.AreEqual(uri.AbsoluteUri, onReceivedData[3].Value<string>());

            onComplete.WaitOne();
            Assert.IsNotNull(onCompleteData);
            AssertNotRequestError(onCompleteData);
            Assert.AreEqual(42, onCompleteData[0].Value<int>());
            Assert.IsNull(onCompleteData[1].Value<string>());
        }

        [Test]
        public void NetworkingModule_Response_Content()
        {
            var onReceived = new AutoResetEvent(false);
            var onReceivedData = default(JArray);

            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                var response = new HttpResponseMessage(HttpStatusCode.Ok);
#else
                var response = new HttpResponseMessage(HttpStatusCode.OK);
#endif
                response.RequestMessage = request;

                var stream = new MemoryStream();

#if WINDOWS_UWP
                response.Content = new HttpStreamContent(stream.AsInputStream());
#else
                response.Content = new StreamContent(stream);
#endif
                using (var streamWriter = new StreamWriter(stream, Encoding.UTF8, 1024, true))
                {
                    streamWriter.Write("Hello World");
                }

                stream.Position = 0;
                return response;
            }),
            new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = args[0] as string;
                    if (eventName == "didReceiveNetworkData")
                    {
                        onReceivedData = args[1] as JArray;
                        onReceived.Set();
                    }
                }
            }));

            var uri = new Uri("http://example.com");
            module.sendRequest("get", uri, 42, null, null, "text", false, 1000);

            onReceived.WaitOne();
            Assert.AreEqual(42, onReceivedData[0].Value<int>());
            Assert.AreEqual("Hello World", onReceivedData[1].Value<string>());
        }

        [Test]
        public void NetworkingModule_Response_NoContent()
        {
            var onReceived = new AutoResetEvent(false);
            var onReceivedData = default(JArray);

            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                var response = new HttpResponseMessage(HttpStatusCode.Ok);
#else
                var response = new HttpResponseMessage(HttpStatusCode.OK);
#endif
                response.RequestMessage = request;

                var stream = new MemoryStream();

#if WINDOWS_UWP
                response.Content = new HttpStreamContent(stream.AsInputStream());
#else
                response.Content = new StreamContent(stream);
#endif

                return response;
            }),
            new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = args[0] as string;
                    if (eventName == "didReceiveNetworkData")
                    {
                        onReceivedData = args[1] as JArray;
                        onReceived.Set();
                    }
                }
            }));

            var uri = new Uri("http://example.com");
            module.sendRequest("get", uri, 42, null, null, "text", false, 1000);

            onReceived.WaitOne();
            Assert.AreEqual(42, onReceivedData[0].Value<int>());
            Assert.AreEqual("", onReceivedData[1].Value<string>());
        }

        [Test]
        public void NetworkingModule_Response_Content_Base64()
        {
            var onReceived = new AutoResetEvent(false);
            var onReceivedData = default(JArray);

            var bytes = Encoding.UTF8.GetBytes("Hello World");
            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                var response = new HttpResponseMessage(HttpStatusCode.Ok);
#else
                var response = new HttpResponseMessage(HttpStatusCode.OK);
#endif
                response.RequestMessage = request;

                var stream = new MemoryStream(bytes);
#if WINDOWS_UWP
                response.Content = new HttpStreamContent(stream.AsInputStream());
#else
                response.Content = new StreamContent(stream);
#endif
                return response;
            }),
            new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = args[0] as string;
                    if (eventName == "didReceiveNetworkData")
                    {
                        onReceivedData = args[1] as JArray;
                        onReceived.Set();
                    }
                }
            }));

            var uri = new Uri("http://example.com");
            module.sendRequest("get", uri, 42, null, null, "base64", false, 1000);

            onReceived.WaitOne();
            Assert.AreEqual(42, onReceivedData[0].Value<int>());
            Assert.AreEqual(Convert.ToBase64String(bytes), onReceivedData[1].Value<string>());
        }

        [Test]
        public void NetworkingModule_Response_NoContent_Base64()
        {
            var onReceived = new AutoResetEvent(false);
            var onReceivedData = default(JArray);

            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                var response = new HttpResponseMessage(HttpStatusCode.Ok);
#else
                var response = new HttpResponseMessage(HttpStatusCode.OK);
#endif
                response.RequestMessage = request;

                var stream = new MemoryStream();
#if WINDOWS_UWP
                response.Content = new HttpStreamContent(stream.AsInputStream());
#else
                response.Content = new StreamContent(stream);
#endif
                return response;
            }),
            new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = args[0] as string;
                    if (eventName == "didReceiveNetworkData")
                    {
                        onReceivedData = args[1] as JArray;
                        onReceived.Set();
                    }
                }
            }));

            var uri = new Uri("http://example.com");
            module.sendRequest("get", uri, 42, null, null, "base64", false, 1000);

            onReceived.WaitOne();
            Assert.AreEqual(42, onReceivedData[0].Value<int>());
            Assert.AreEqual("", onReceivedData[1].Value<string>());
        }

        [Test]
        public void NetworkingModule_Response_Content_UseIncremental()
        {
            var onCompleteData = default(JArray);
            var onReceived = new AutoResetEvent(false);
            var builder = new StringBuilder();
            var size = 32 * 1027;
            var expected = new string(Enumerable.Repeat('a', size).ToArray());

            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
#if WINDOWS_UWP
                var response = new HttpResponseMessage(HttpStatusCode.Ok);
#else
                var response = new HttpResponseMessage(HttpStatusCode.OK);
#endif
                response.RequestMessage = request;

                var stream = new MemoryStream();

#if WINDOWS_UWP
                response.Content = new HttpStreamContent(stream.AsInputStream());
#else
                response.Content = new StreamContent(stream);
#endif
                using (var streamWriter = new StreamWriter(stream, Encoding.UTF8, 1024, true))
                {
                    streamWriter.Write(expected);
                }

                stream.Position = 0;
                return response;
            }),
            new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = args[0] as string;
                    if (eventName == "didReceiveNetworkIncrementalData")
                    {
                        builder.Append(((JArray)args[1])[1].Value<string>());
                    }
                    else if (eventName == "didCompleteNetworkResponse")
                    {
                        onCompleteData = args[1] as JArray;
                        onReceived.Set();
                    }
                }
            }));

            var uri = new Uri("http://example.com");
            module.sendRequest("get", uri, 0, null, null, "text", true, 1000);

            onReceived.WaitOne();
            Assert.IsNotNull(onCompleteData);
            AssertNotRequestError(onCompleteData);
            Assert.AreEqual(expected, builder.ToString());
        }

        [Test]
        public void NetworkingModule_Response_RedirectedUrl()
        {
            var uri = new Uri("http://example.com");
            var redirectedUri = new Uri("http://foo.com/api?bar=123&token=abcd");

            var onReceived = new AutoResetEvent(false);
            var onReceivedData = default(JArray);

            var module = CreateNetworkingModule(new MockHttpClient(request =>
            {
                // simulating redirect
                request.RequestUri = redirectedUri;
#if WINDOWS_UWP
                var response = new HttpResponseMessage(HttpStatusCode.Ok);
#else
                var response = new HttpResponseMessage(HttpStatusCode.OK);
#endif
                response.RequestMessage = request;

                return response;
            }),
            new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = args[0] as string;
                    if (eventName == "didReceiveNetworkResponse")
                    {
                        onReceivedData = args[1] as JArray;
                        onReceived.Set();
                    }
                }
            }));

            module.sendRequest("get", uri, 42, null, null, "text", false, 1000);

            onReceived.WaitOne();
            Assert.AreEqual(42, onReceivedData[0].Value<int>());
            Assert.AreEqual(redirectedUri.AbsoluteUri, onReceivedData[3].Value<string>());
        }

        private static void AssertNotRequestError(JArray onCompleteData)
        {
            if (onCompleteData.Count != 2 && onCompleteData.Count != 3)
            {
                Assert.Fail("Invalid completion response from NetworkingModule.");
            }

            if (onCompleteData.Count != 2)
            {
                Assert.Fail("Network request failed with message: " + onCompleteData[1]);
            }
        }

        private static NetworkingModule CreateNetworkingModule(IHttpClient httpClient, IInvocationHandler handler)
        {
            var context = new ReactContext();

            var eventEmitter = new RCTDeviceEventEmitter();
            eventEmitter.InvocationHandler = handler;

            var reactInstance = new TestReactInstance(eventEmitter);
            context.InitializeWithInstance(reactInstance);
            return new NetworkingModule(httpClient, context);
        }

        class TestReactInstance : MockReactInstance
        {
            private readonly object _eventEmitter;

            public TestReactInstance(RCTDeviceEventEmitter eventEmitter)
                : base()
            {
                _eventEmitter = eventEmitter;
            }

            public override T GetJavaScriptModule<T>()
            {
                if (typeof(RCTDeviceEventEmitter) == typeof(T))
                {
                    return (T)_eventEmitter;
                }

                return base.GetJavaScriptModule<T>();
            }
        }

        class MockHttpClient : IHttpClient
        {
            private readonly Func<HttpRequestMessage, HttpResponseMessage> _func;

            public MockHttpClient(Func<HttpRequestMessage, HttpResponseMessage> func)
            {
                _func = func;
            }

            public Task<HttpResponseMessage> SendRequestAsync(HttpRequestMessage request, CancellationToken token)
            {
                return Task.FromResult(_func(request));
            }

            public void Dispose()
            {
            }
        }
    }
}
