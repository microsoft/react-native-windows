// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.Modules.WebSocket;
using System;
using System.Text;
using System.Threading;

namespace ReactNative.Tests.Modules.WebSocket
{
    [TestFixture]
    public class WebSocketModuleTests
    {
        private const int WaitTimeoutInMs = 5000;

        [Test]
        [Category("Network")]
        public void WebSocketModule_OpenClosedEvent()
        {
            var waitHandle = new AutoResetEvent(false);
            var openParams = default(JObject);
            var closeParams = default(JObject);
            var context = CreateReactContext(new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = (string)args[0];
                    switch (eventName)
                    {
                        case "websocketClosed":
                            closeParams = (JObject)args[1];
                            waitHandle.Set();
                            break;
                        case "websocketOpen":
                            openParams = (JObject)args[1];
                            waitHandle.Set();
                            break;
                    }
                }
            }));

            var module = new WebSocketModule(context);
            try
            {
                module.connect("ws://echo.websocket.org", null, null, 1);
                Assert.IsTrue(waitHandle.WaitOne());
            }
            finally
            {
                module.close(1000, "None", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
                waitHandle.Dispose();
            }

            Assert.AreEqual(1, openParams["id"].Value<int>());
            Assert.AreEqual(1, closeParams["id"].Value<int>());
            Assert.AreEqual(1000, closeParams["code"].Value<int>());
            Assert.AreEqual("None", closeParams["reason"].Value<string>());
        }

        [Test]
        public void WebSocketModule_FailedEvent()
        {
            var waitHandle = new AutoResetEvent(false);
            var json = default(JObject);
            var context = CreateReactContext(new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2)
                {
                    var eventName = (string)args[0];
                    switch (eventName)
                    {
                        case "websocketFailed":
                            json = (JObject)args[1];
                            waitHandle.Set();
                            break;
                    }
                }
            }));

            var module = new WebSocketModule(context);
            try
            {
                module.connect("ws://invalid.websocket.address", null, null, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
            }
            finally
            {
                module.close(1000, "None", 1);
            }

            Assert.AreEqual(1, json["id"].Value<int>());
        }

        [Test]
        [Category("Network")]
        public void WebSocketModule_DataEvent()
        {
            var waitHandle = new AutoResetEvent(false);
            var json = default(JObject);
            var context = CreateReactContext(new MockInvocationHandler((name, args) =>
            {
                var eventName = (string)args[0];
                switch (eventName)
                {
                    case "websocketOpen":
                    case "websocketClosed":
                        waitHandle.Set();
                        break;
                    case "websocketMessage":
                        json = (JObject)args[1];
                        waitHandle.Set();
                        break;
                }
            }));

            var module = new WebSocketModule(context);
            try
            {
                module.connect("ws://echo.websocket.org", null, null, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
                module.send("FooBarBaz", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
            }
            finally
            {
                module.close(1000, "None", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));

                waitHandle.Dispose();
            }

            Assert.AreEqual(1, json["id"].Value<int>());
            Assert.AreEqual("text", json["type"].Value<string>());
            Assert.AreEqual("FooBarBaz", json["data"].Value<string>());
        }

        [Test]
        [Category("Network")]
        public void WebSocketModule_DataEvent_Binary()
        {
            var waitHandle = new AutoResetEvent(false);
            var json = default(JObject);
            var context = CreateReactContext(new MockInvocationHandler((name, args) =>
            {
                var eventName = (string)args[0];
                switch (eventName)
                {
                    case "websocketOpen":
                    case "websocketClosed":
                        waitHandle.Set();
                        break;
                    case "websocketMessage":
                        json = (JObject)args[1];
                        waitHandle.Set();
                        break;
                }
            }));

            var module = new WebSocketModule(context);
            var encodedMessage = Convert.ToBase64String(Encoding.UTF8.GetBytes("Hello World"));
            try
            {
                module.connect("ws://echo.websocket.org", null, null, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
                module.sendBinary(encodedMessage, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
            }
            finally
            {
                module.close(1000, "None", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));

                waitHandle.Dispose();
            }

            Assert.AreEqual(1, json["id"].Value<int>());
            Assert.AreEqual("binary", json["type"].Value<string>());
            Assert.AreEqual(encodedMessage, json["data"].Value<string>());
        }

        [Test]
        [Category("Network")]
        public void WebSocketModule_DataEvent_Binary_ThenText()
        {
            var waitHandle = new AutoResetEvent(false);
            var json = default(JObject);
            var context = CreateReactContext(new MockInvocationHandler((name, args) =>
            {
                var eventName = (string)args[0];
                switch (eventName)
                {
                    case "websocketOpen":
                    case "websocketClosed":
                        waitHandle.Set();
                        break;
                    case "websocketMessage":
                        json = (JObject)args[1];
                        waitHandle.Set();
                        break;
                }
            }));

            var module = new WebSocketModule(context);
            var encodedMessage = Convert.ToBase64String(Encoding.UTF8.GetBytes("Hello World"));
            try
            {
                module.connect("ws://echo.websocket.org", null, null, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
                module.sendBinary(encodedMessage, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
                module.send("FooBarBaz", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
            }
            finally
            {
                module.close(1000, "None", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));

                waitHandle.Dispose();
            }

            Assert.AreEqual(1, json["id"].Value<int>());
            Assert.AreEqual("text", json["type"].Value<string>());
            Assert.AreEqual("FooBarBaz", json["data"].Value<string>());
        }

        [Test]
        [Category("Network")]
        public void WebSocketModule_DataEvent_Text_ThenBinary()
        {
            var waitHandle = new AutoResetEvent(false);
            var json = default(JObject);
            var context = CreateReactContext(new MockInvocationHandler((name, args) =>
            {
                var eventName = (string)args[0];
                switch (eventName)
                {
                    case "websocketOpen":
                    case "websocketClosed":
                        waitHandle.Set();
                        break;
                    case "websocketMessage":
                        json = (JObject)args[1];
                        waitHandle.Set();
                        break;
                }
            }));

            var module = new WebSocketModule(context);
            var encodedMessage = Convert.ToBase64String(Encoding.UTF8.GetBytes("Hello World"));
            try
            {
                module.connect("ws://echo.websocket.org", null, null, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
                module.send("FooBarBaz", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
                module.sendBinary(encodedMessage, 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));
            }
            finally
            {
                module.close(1000, "None", 1);
                Assert.IsTrue(waitHandle.WaitOne(WaitTimeoutInMs));

                waitHandle.Dispose();
            }

            Assert.AreEqual(1, json["id"].Value<int>());
            Assert.AreEqual("binary", json["type"].Value<string>());
            Assert.AreEqual(encodedMessage, json["data"].Value<string>());
        }

        private ReactContext CreateReactContext(IInvocationHandler handler)
        {
            var eventEmitter = new RCTDeviceEventEmitter();
            eventEmitter.InvocationHandler = handler;
            var reactInstance = new TestReactInstance(eventEmitter);
            var reactContext = new ReactContext();
            reactContext.InitializeWithInstance(reactInstance);
            return reactContext;
        }

        class TestReactInstance : MockReactInstance
        {
            private readonly object _eventEmitter;

            public TestReactInstance(RCTDeviceEventEmitter eventEmitter)
            {
                _eventEmitter = eventEmitter;
            }

            public override T GetJavaScriptModule<T>()
            {
                if (typeof(T) == typeof(RCTDeviceEventEmitter))
                {
                    return (T)_eventEmitter;
                }

                return base.GetJavaScriptModule<T>();
            }
        }
    }
}
