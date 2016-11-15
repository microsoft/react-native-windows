using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.Modules.NetInfo;
using System;
using System.Net.NetworkInformation;
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
            var networkInterface = new MockNetworkInterface("None");
            var networkInfo = new MockNetworkInformation(networkInterface);
            var context = CreateReactContext();
            var netInfo = new NetInfoModule(networkInfo, context);

            var state = default(JObject);
            var promise = new MockPromise(value => state = (JObject)value);

            netInfo.getCurrentConnectivity(promise);
            Assert.AreEqual(CreateNetworkInfo("None"), state);

            networkInfo.NetworkInterface = new MockNetworkInterface("InternetAccess");
            netInfo.getCurrentConnectivity(promise);
            Assert.AreEqual(CreateNetworkInfo("InternetAccess"), state);
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void NetInfoModule_Event()
        {
            SetDispatcherForTest();

            var networkInterface = new MockNetworkInterface("None");
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

            networkInfo.NetworkInterface = new MockNetworkInterface("InternetAccess");
            networkInfo.OnNetworkAvailabilityChanged(new object(), (NetworkAvailabilityEventArgs)null);
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
            ReactNative.Bridge.DispatcherHelpers.CurrentDispatcher = Dispatcher.CurrentDispatcher;
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

            public MockNetworkInterface NetworkInterface { get; set; }

            public MockNetworkInformation(MockNetworkInterface NetworkInterface)
                : this(() => { }, () => { })
            {
                this.NetworkInterface = NetworkInterface;
            }

            public MockNetworkInformation(Action onStart, Action onStop)
            {
                _onStart = onStart;
                _onStop = onStop;
            }

            public event NetworkAvailabilityChangedEventHandler NetworkAvailabilityChanged;

            public string GetInternetStatus()
            {
                return NetworkInterface.GetIsNetworkAvailable();
            }

            public void Start()
            {
                _onStart();
            }

            public void Stop()
            {
                _onStop();
            }

            public void OnNetworkAvailabilityChanged(object source, NetworkAvailabilityEventArgs e)
            {
                NetworkAvailabilityChanged?.Invoke(source, e);
            }
        }

        class MockNetworkInterface
        {
            public MockNetworkInterface(string status)
            {
                _status = status;
            }

            private string _status;

            public string GetIsNetworkAvailable()
            {
                return _status;
            }
        }
    }
}
