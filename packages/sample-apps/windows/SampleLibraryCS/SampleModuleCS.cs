// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;
using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Security.Cryptography.Core;
using Windows.System.Threading;

namespace SampleLibraryCS
{
    class Point
    {
        public int x = 0;
        public int y = 0;
    };

    // Sample ReactModule

    [ReactModule]
    internal sealed class SampleModuleCS
    {
        #region Initializer

        [ReactInitializer]
        public void Initialize(ReactContext reactContext)
        {
            _reactContext = reactContext;

            Debug.WriteLine($"C# Properties.Prop1: {reactContext.Handle.Properties.Get(ReactPropertyBagHelper.GetName(null, "Prop1"))}");
            Debug.WriteLine($"C# Properties.Prop2: {reactContext.Handle.Properties.Get(ReactPropertyBagHelper.GetName(null, "Prop2"))}");

            var cppTimerNotification = ReactPropertyBagHelper.GetName(ReactPropertyBagHelper.GetNamespace("SampleModuleCppImpl"), "TimerNotification");
            var csTimerNotification = ReactPropertyBagHelper.GetName(ReactPropertyBagHelper.GetNamespace("SampleModuleCS"), "TimerNotification");

            reactContext.Handle.Notifications.Subscribe(cppTimerNotification, null,
                (object sender, IReactNotificationArgs args) => Debug.WriteLine($"C# module, C++ timer:: {args.Data}"));

            _timer = ThreadPoolTimer.CreatePeriodicTimer(new TimerElapsedHandler((timer) =>
            {
                reactContext.Handle.Notifications.SendNotification(csTimerNotification, null, _timerCount);
                TimedEvent?.Invoke(++_timerCount);
                if (_timerCount == 5)
                {
                    _timer?.Cancel();
                }
            }),
            TimeSpan.FromMilliseconds(TimedEventIntervalMS));
        }

        ~SampleModuleCS()
        {
            _timer?.Cancel();
        }

        #endregion

        #region Constants

        [ReactConstant]
        public double NumberConstant => Math.PI;

        [ReactConstant]
        public string StringConstant => "Hello World";

        [ReactConstantProvider]
        public void ConstantsViaConstantsProvider(ReactConstantProvider provider)
        {
            provider.Add("NumberConstantViaProvider", Math.PI);
            provider.Add("StringConstantViaProvider", "Hello World");
        }

        #endregion

        #region Methods

        [ReactMethod]
        public void VoidMethod()
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(VoidMethod)}()");
        }

        [ReactMethod]
        public void VoidMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(VoidMethodWithArgs)}({arg})");
        }

        [ReactMethod]
        public double ReturnMethod()
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ReturnMethod)}()");
            return Math.PI;
        }

        [ReactMethod]
        public double ReturnMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ReturnMethodWithArgs)}({arg})");
            return Math.PI;
        }

        #endregion

        #region Methods using ReactCallbacks

        [ReactMethod]
        public void ExplicitCallbackMethod(Action<double> callback)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ExplicitCallbackMethod)}()");
            callback(Math.PI);
        }

        [ReactMethod]
        public void ExplicitCallbackMethodWithArgs(double arg, Action<double> callback)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ExplicitCallbackMethodWithArgs)}({arg})");
            callback(Math.PI);
        }

        // Use callback order as with ReactPromise
        [ReactMethod]
        public void TwoCallbacksMethod(bool shouldSucceed, Action<string> onSuccess, Action<string> onFailure)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(TwoCallbacksMethod)}({shouldSucceed})");
            if (shouldSucceed)
            {
                onSuccess("TwoCallbacksMethod succeeded");
            }
            else
            {
                onFailure("TwoCallbacksMethod failed");
            }
        }

        // Use callback order as with ReactPromise
        [ReactMethod]
        public async void TwoCallbacksAsyncMethod(bool shouldSucceed, Action<string> onSuccess, Action<string> onFailure)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(TwoCallbacksAsyncMethod)}({shouldSucceed})");
            await Task.Run(() => { });
            if (shouldSucceed)
            {
                onSuccess("TwoCallbacksMethod succeeded");
            }
            else
            {
                onFailure("TwoCallbacksMethod failed");
            }
        }

        // Use callback order as in "classic" ReactNative.
        [ReactMethod]
        public void ReverseTwoCallbacksMethod(bool shouldSucceed, Action<string> onFailure, Action<string> onSuccess)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ReverseTwoCallbacksMethod)}({shouldSucceed})");
            if (shouldSucceed)
            {
                onSuccess("ReverseTwoCallbacksMethod succeeded");
            }
            else
            {
                onFailure("ReverseTwoCallbacksMethod failed");
            }
        }

        // Use callback order as in "classic" ReactNative.
        [ReactMethod]
        public async void ReverseTwoCallbacksAsyncMethod(bool shouldSucceed, Action<string> onFailure, Action<string> onSuccess)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ReverseTwoCallbacksMethod)}({shouldSucceed})");
            await Task.Run(() => { });
            if (shouldSucceed)
            {
                onSuccess("ReverseTwoCallbacksMethod succeeded");
            }
            else
            {
                onFailure("ReverseTwoCallbacksMethod failed");
            }
        }

        [ReactMethod]
        public void ExplicitPromiseMethod(IReactPromise<double> result)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ExplicitPromiseMethod)}()");

            try
            {
                result.Resolve(Math.PI);
            }
            catch (Exception ex)
            {
                result.Reject(new ReactError { Message = ex.Message });
            }
        }

        [ReactMethod]
        public void ExplicitPromiseMethodWithArgs(double arg, IReactPromise<double> result)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ExplicitPromiseMethodWithArgs)}({arg})");

            try
            {
                result.Resolve(Math.PI);
            }
            catch (Exception ex)
            {
                result.Reject(new ReactError { Message = ex.Message });
            }
        }

        [ReactMethod]
        public async void NegateAsyncPromise(int x, IReactPromise<int> result)
        {
            bool isPosititve = await Task.Run(() => x >= 0);
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(NegateAsyncPromise)}({x})");
            if (isPosititve)
            {
                result.Resolve(-x);
            }
            else
            {
                result.Reject(new ReactError { Message = "Already negative" });
            }
        }

        #endregion

        #region Methods using Task

        [ReactMethod]
        public Task TaskNoArgs()
        {
            return Task<int>.Factory.StartNew(() => 42);
        }

        [ReactMethod]
        public async Task TaskTwoArgs(int x, int y)
        {
            await Task.Delay(TimeSpan.FromMilliseconds(250));
        }

        [ReactMethod]
        public Task<int> TaskOfTNoArgs()
        {
            return Task<int>.Factory.StartNew(() => 42);
        }

        [ReactMethod]
        public async Task<int> TaskOfTTwoArgs(int x, int y)
        {
            await Task.Delay(TimeSpan.FromMilliseconds(250));
            return x + y;
        }

        #endregion

        #region Synchronous Methods

        [ReactSyncMethod]
        public double SyncReturnMethod()
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(SyncReturnMethod)}()");
            return Math.PI;
        }

        [ReactSyncMethod]
        public double SyncReturnMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(SyncReturnMethodWithArgs)}({arg})");
            return Math.PI;
        }

        #endregion

        #region Events

        [ReactEvent("TimedEventCS")]
        public Action<int> TimedEvent { get; set; }

        [ReactMethod]
        public void EmitJSEvent1(int value)
        {
            // Test the ReactContext.EmitJSEvent
            _reactContext.EmitJSEvent("RCTDeviceEventEmitter", "EmitJSEvent1CS", value);
        }

        [ReactMethod]
        public void EmitJSEvent2(int value1, int value2)
        {
            // Test the ReactContext::EmitJSEvent
            _reactContext.EmitJSEvent("RCTDeviceEventEmitter", "EmitJSEvent2CS", value1, value2);
        }

        [ReactMethod]
        public void EmitJSEvent3(int value1, int value2)
        {
            // Test the ReactContext::EmitJSEvent
            _reactContext.EmitJSEvent<JSValueArgWriter>("RCTDeviceEventEmitter", "EmitJSEvent3CS", (IJSValueWriter argWriter) =>
            {
                argWriter.WriteValue(value1);
                argWriter.WriteValue(value2);
            });
        }

        #endregion

        #region JS Functions

        [ReactFunction("calcDistance", ModuleName = "SampleModuleCpp")]
        public Action<Point, Point> CalcDistance = null;

        [ReactMethod("callDistanceFunction")]
        public void CallDistanceFunction(Point point1, Point point2)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(CallDistanceFunction)}()");
            CalcDistance(point1, point2);
        }

        #endregion

        [ReactMethod]
        public void ReloadInstance()
        {
            ReactNativeHost.FromContext(_reactContext.Handle).ReloadInstance();
        }

        private ThreadPoolTimer _timer;
        private int _timerCount = 0;
        private const int TimedEventIntervalMS = 5000;
        private ReactContext _reactContext;
    }
}
