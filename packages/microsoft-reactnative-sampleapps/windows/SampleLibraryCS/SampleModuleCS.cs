// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;

using Windows.System.Threading;

using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed;

namespace SampleLibraryCS
{
    // Sample ReactModule

    [ReactModule]
    internal sealed class SampleModuleCS
    {
        public string Name => nameof(SampleModuleCS);

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
            Debug.WriteLine($"{Name}.{nameof(VoidMethod)}()");
        }

        [ReactMethod]
        public void VoidMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(VoidMethodWithArgs)}({arg})");
        }

        [ReactMethod]
        public double ReturnMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(ReturnMethod)}()");
            return Math.PI;
        }

        [ReactMethod]
        public double ReturnMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(ReturnMethodWithArgs)}({arg})");
            return Math.PI;
        }

        #endregion

        #region Methods using ReactCallBacks

        [ReactMethod]
        public void ExplicitCallbackMethod(ReactCallback<double> callback)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitCallbackMethod)}()");
            callback(Math.PI);
        }

        [ReactMethod]
        public void ExplicitCallbackMethodWithArgs(double arg, ReactCallback<double> callback)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitCallbackMethodWithArgs)}({arg})");
            callback(Math.PI);
        }

        [ReactMethod]
        public void ExplicitPromiseMethod(ReactCallback<double> resolve, ReactCallback<string> reject)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitPromiseMethod)}()");

            try
            {
                resolve(Math.PI);
            }
            catch (Exception ex)
            {
                reject(ex.Message);
            }
        }

        [ReactMethod]
        public void ExplicitPromiseMethodWithArgs(double arg, ReactCallback<double> resolve, ReactCallback<string> reject)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitPromiseMethodWithArgs)}({arg})");

            try
            {
                resolve(Math.PI);
            }
            catch (Exception ex)
            {
                reject(ex.Message);
            }
        }

        #endregion

        #region Synchronous Methods

        [ReactSyncMethod]
        public double SyncReturnMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(SyncReturnMethod)}()");
            return Math.PI;
        }

        [ReactSyncMethod]
        public double SyncReturnMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(SyncReturnMethodWithArgs)}({arg})");
            return Math.PI;
        }

        #endregion

        #region Events

        [ReactEvent]
        public ReactEvent<int> TimedEvent { get; set; }

        [ReactMethod]
        public int StartTimedEvent(int intervalMS)
        {
            Debug.WriteLine($"{Name}.{nameof(StartTimedEvent)}({intervalMS})");

            int id = _timers.Count;

            _timers.Add(ThreadPoolTimer.CreatePeriodicTimer(new TimerElapsedHandler((timer) =>
            {
                TimedEvent?.Invoke(id);
            }),
            TimeSpan.FromMilliseconds(intervalMS)));

            return id;
        }

        private List<ThreadPoolTimer> _timers = new List<ThreadPoolTimer>();

        #endregion

        ~SampleModuleCS()
        {
            foreach(var timer in _timers)
            {
                timer.Cancel();
            }
        }
    }
}
