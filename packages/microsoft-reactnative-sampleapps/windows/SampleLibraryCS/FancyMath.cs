// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;

using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed;

namespace SampleLibraryCS
{
    // Example using IManagedNativeModule and attributes (recommended)
    public sealed class FancyMath : IManagedNativeModule
    {
        public string Name => "FancyMath";

        public void Initialize()
        {
            Debug.WriteLine("FancyMath.Initialize()");
        }

        [NativeModuleConstant]
        public double E => Math.E;

        [NativeModuleConstant(Name = "Pi")]
        public double PI => Math.PI;

        [NativeModuleMethod(Name = "add")]
        public static double Add(double a, double b)
        {
            return a + b;
        }
    }

    // Example using INativeModule directly (ABI-safe but not recommended)
    public sealed class FancyMathABI : INativeModule
    {
        public string Name => "FancyMathABI";

        public void Initialize()
        {
            Debug.WriteLine("FancyMathABI.Initialize()");
        }

        public IReadOnlyDictionary<string, object> Constants => _constants ?? (_constants = new Dictionary<string, object>(2)
        {
            { "E", Math.E },
            { "Pi", Math.PI },
        });
        private IReadOnlyDictionary<string, object> _constants;

        public IReadOnlyList<MethodInfo> Methods => _methods ?? (_methods = new List<MethodInfo>(1)
        {
            new MethodInfo("add", ReturnType.Callback, (args, callback, ___) =>
            {
                double a = (double)args[0];
                double b = (double)args[1];

                double result = a + b;

                callback(new object[] { result });
            }),
        });
        private IReadOnlyList<MethodInfo> _methods;
    }
}
