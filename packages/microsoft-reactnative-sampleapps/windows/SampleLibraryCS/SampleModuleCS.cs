// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;
using System;
using System.Diagnostics;
using System.Reflection;
using System.Threading.Tasks;
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
        public void Initialize(IReactContext reactContext)
        {
            _timer = ThreadPoolTimer.CreatePeriodicTimer(new TimerElapsedHandler((timer) =>
            {
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
        public void ExplicitCallbackMethod(ReactCallback<double> callback)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ExplicitCallbackMethod)}()");
            callback(Math.PI);
        }

        [ReactMethod]
        public void ExplicitCallbackMethodWithArgs(double arg, ReactCallback<double> callback)
        {
            Debug.WriteLine($"{nameof(SampleModuleCS)}.{nameof(ExplicitCallbackMethodWithArgs)}({arg})");
            callback(Math.PI);
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

        private ThreadPoolTimer _timer;
        private int _timerCount = 0;
        private const int TimedEventIntervalMS = 5000;
    }
}
