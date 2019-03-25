// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NETWORKLIST;
using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.Modules.NetInfo;
using System;
using System.Threading;
using System.Windows.Threading;

namespace ReactNative.Tests.Modules.NetInfo
{
    [TestFixture]
    public class NetInfoModuleTests
    {
        [Test]
        public void NetInfoModule_JsonResponse()
        {
            var networkInterface = new MockNetworkListManager("None");
            var networkInfo = new MockNetworkInformation(networkInterface);
            var context = CreateReactContext();
            var netInfo = new NetInfoModule(networkInfo, context);

            var state = default(JObject);
            var promise = new MockPromise(value => state = (JObject)value);

            netInfo.getCurrentConnectivity(promise);
            Assert.AreEqual(CreateNetworkInfo("None"), state);

            networkInfo.networkListManager = new MockNetworkListManager("InternetAccess");
            netInfo.getCurrentConnectivity(promise);
            Assert.AreEqual(CreateNetworkInfo("InternetAccess"), state);
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void NetInfoModule_Event()
        {
            SetDispatcherForTest();

            var networkInterface = new MockNetworkListManager("None");
            var networkInfo = new MockNetworkInformation(networkInterface);

            var emitted = new AutoResetEvent(false);
            var state = default(JObject);
            var context = CreateReactContext(new MockInvocationHandler((name, args) =>
            {
                if (name == "emit" && args.Length == 2 && ((string)args[0]) == "networkStatusDidChange")
                {
                    state = (JObject)args[1];
                    emitted.Set();
                }
            }));

            var netInfo = new NetInfoModule(networkInfo, context);
            netInfo.Initialize();
            context.OnResume();

            networkInfo.networkListManager = new MockNetworkListManager("InternetAccess");
            networkInfo.OnNetworkAvailabilityChanged(new Guid(), NLM_CONNECTIVITY.NLM_CONNECTIVITY_IPV4_INTERNET);
            Assert.IsTrue(emitted.WaitOne());
            Assert.AreEqual(CreateNetworkInfo("InternetAccess"), state);
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void NetInfoModule_LifecycleChecks()
        {
            SetDispatcherForTest();

            var started = new AutoResetEvent(false);
            var stopped = new AutoResetEvent(false);

            var networkInfo = new MockNetworkInformation(
                () => started.Set(),
                () => stopped.Set());

            var context = CreateReactContext();
            var netInfo = new NetInfoModule(networkInfo, context);
            netInfo.Initialize();

            context.OnResume();
            Assert.IsTrue(started.WaitOne());

            context.OnSuspend();
            Assert.IsTrue(stopped.WaitOne());
        }

        private static JObject CreateNetworkInfo(string status)
        {
            return new JObject
            {
                { "network_info", status },
            };
        }

        private static ReactContext CreateReactContext()
        {
            return CreateReactContext(new MockInvocationHandler());
        }

        private static ReactContext CreateReactContext(IInvocationHandler handler)
        {
            var context = new ReactContext();
            var eventEmitter = new RCTDeviceEventEmitter();
            eventEmitter.InvocationHandler = handler;

            var reactInstance = new TestReactInstance(eventEmitter);
            context.InitializeWithInstance(reactInstance);
            return context;
        }

        private static void SetDispatcherForTest()
        {
            ReactNative.Bridge.DispatcherHelpers.MainDispatcher = Dispatcher.CurrentDispatcher;
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

        class MockNetworkInformation : INetworkInformation
        {
            private readonly Action _onStart;
            private readonly Action _onStop;

            public MockNetworkListManager networkListManager { get; set; }

            public MockNetworkInformation(MockNetworkListManager networkListManager)
                : this(() => { }, () => { })
            {
                this.networkListManager = networkListManager;
            }

            public MockNetworkInformation(Action onStart, Action onStop)
            {
                _onStart = onStart;
                _onStop = onStop;
            }

            public event EventHandler<NetworkConnectivityChangedEventArgs> NetworkConnectivityChanged;

            public string GetInternetStatus()
            {
                return networkListManager.IsConnectedToInternet ? "InternetAccess" : "None";
            }

            public void Start()
            {
                _onStart();
            }

            public void Stop()
            {
                _onStop();
            }

            public void OnNetworkAvailabilityChanged(Guid guid, NLM_CONNECTIVITY connectivity)
            {
                NetworkConnectivityChangedEventArgs e = new NetworkConnectivityChangedEventArgs()
                {
                    IsAvailable = networkListManager.IsConnectedToInternet,
                    ConnectionStatus = GetInternetStatus()
                };
                NetworkConnectivityChanged?.Invoke(new object(), e);
            }
        }

        class MockNetworkListManager
        {
            public MockNetworkListManager(string status)
            {
                IsConnectedToInternet = status == "InternetAccess";
            }

            public bool IsConnectedToInternet { get; set; }
        }
    }
}
