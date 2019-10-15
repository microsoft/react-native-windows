// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;

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
    }
}
