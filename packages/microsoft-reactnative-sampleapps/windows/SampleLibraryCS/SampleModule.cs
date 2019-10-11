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
    public sealed class SampleModule : IManagedNativeModule
    {
        #region IManagedNativeModule

        public string Name => nameof(SampleModule);

        public void Initialize()
        {
            Debug.WriteLine($"{Name}.{nameof(Initialize)}()");
        }

        #endregion

        #region Constants

        [NativeModuleConstant]
        public double NumberConstant => Math.PI;

        [NativeModuleConstant]
        public string StringConstant => "Hello World";

        #endregion

        #region Methods

        [NativeModuleMethod]
        public void VoidMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(VoidMethod)}()");
        }

        [NativeModuleMethod]
        public void VoidMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(VoidMethodWithArgs)}({arg})");
        }

        [NativeModuleMethod]
        public double ReturnMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(ReturnMethod)}()");
            return Math.PI;
        }

        [NativeModuleMethod]
        public double ReturnMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(ReturnMethodWithArgs)}({arg})");
            return Math.PI;
        }

        #endregion

        #region Methods with explicit ReturnTypes

        [NativeModuleMethod(ReturnType = ReturnType.Void)]
        public void ExplicitVoidMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitVoidMethod)}()");
        }

        [NativeModuleMethod(ReturnType = ReturnType.Void)]
        public void ExplicitVoidMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitVoidMethodWithArgs)}({arg})");
        }

        [NativeModuleMethod(ReturnType = ReturnType.Callback)]
        public void ExplicitCallbackMethod(Callback callback)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitCallbackMethod)}()");
            callback(new object[] { Math.PI });
        }

        [NativeModuleMethod(ReturnType = ReturnType.Callback)]
        public void ExplicitCallbackMethodWithArgs(double arg, Callback callback)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitCallbackMethodWithArgs)}({arg})");
            callback(new object[] { Math.PI });
        }

        [NativeModuleMethod(ReturnType = ReturnType.Promise)]
        public void ExplicitPromiseMethod(Callback resolve, Callback reject)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitPromiseMethod)}()");

            try
            {
                resolve(new object[] { Math.PI });
            }
            catch (Exception ex)
            {
                reject(new object[] { "0x80000000", ex.Message, ex.StackTrace, ex });
            }
        }

        [NativeModuleMethod(ReturnType = ReturnType.Promise)]
        public void ExplicitPromiseMethodWithArgs(double arg, Callback resolve, Callback reject)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitPromiseMethodWithArgs)}({arg})");

            try
            {
                resolve(new object[] { Math.PI });
            }
            catch (Exception ex)
            {
                reject(new object[] { "0x80000000", ex.Message, ex.StackTrace, ex });
            }
        }

        #endregion
    }

    // Example using INativeModule directly (ABI-safe but not recommended)
    public sealed class SampleModuleABI : INativeModule
    {
        #region INativeModule

        public string Name => nameof(SampleModuleABI);

        public void Initialize()
        {
            Debug.WriteLine($"{Name}.{nameof(Initialize)}()");
        }

        public IReadOnlyDictionary<string, object> Constants => _constants ?? (_constants = new Dictionary<string, object>(2)
        {
            { nameof(NumberConstant), NumberConstant },
            { nameof(StringConstant), StringConstant },
        });
        private IReadOnlyDictionary<string, object> _constants;

        public IReadOnlyList<MethodInfo> Methods => _methods ?? (_methods = new List<MethodInfo>(1)
        {
            new MethodInfo(nameof(VoidMethod), ReturnType.Void, (_, __, ___) =>
            {
                VoidMethod();
            }),
            new MethodInfo(nameof(VoidMethodWithArgs), ReturnType.Void, (args, __, ___) =>
            {
                double arg = (double)args[0];

                VoidMethodWithArgs(arg);
            }),
            new MethodInfo(nameof(ReturnMethod), ReturnType.Callback, (args, callback, ___) =>
            {
                callback(new object[] { ReturnMethod() });
            }),

            new MethodInfo(nameof(ReturnMethodWithArgs), ReturnType.Callback, (args, callback, ___) =>
            {
                double arg = (double)args[0];

                callback(new object[] { ReturnMethodWithArgs(arg) });
            }),
            new MethodInfo(nameof(ExplicitVoidMethod), ReturnType.Void, (_, __, ___) =>
            {
                ExplicitVoidMethod();
            }),
            new MethodInfo(nameof(ExplicitVoidMethodWithArgs), ReturnType.Void, (args, __, ___) =>
            {
                double arg = (double)args[0];

                ExplicitVoidMethodWithArgs(arg);
            }),
            new MethodInfo(nameof(ExplicitCallbackMethod), ReturnType.Callback, (args, callback, ___) =>
            {
                ExplicitCallbackMethod(callback);
            }),
            new MethodInfo(nameof(ExplicitCallbackMethodWithArgs), ReturnType.Callback, (args, callback, ___) =>
            {
                double arg = (double)args[0];

                ExplicitCallbackMethodWithArgs(arg, callback);
            }),
            new MethodInfo(nameof(ExplicitPromiseMethod), ReturnType.Promise, (args, resolve, reject) =>
            {
                ExplicitPromiseMethod(resolve, reject);
            }),
            new MethodInfo(nameof(ExplicitPromiseMethodWithArgs), ReturnType.Promise, (args, resolve, reject) =>
            {
                double arg = (double)args[0];

                ExplicitPromiseMethodWithArgs(arg, resolve, reject);
            }),
        });
        private IReadOnlyList<MethodInfo> _methods;

        #endregion

        private double NumberConstant => Math.PI;

        private string StringConstant => "Hello World";

        private void VoidMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(VoidMethod)}()");
        }

        private void VoidMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(VoidMethodWithArgs)}({arg})");
        }

        private double ReturnMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(ReturnMethod)}()");
            return Math.PI;
        }

        private double ReturnMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(ReturnMethodWithArgs)}({arg})");
            return Math.PI;
        }

        private void ExplicitVoidMethod()
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitVoidMethod)}()");
        }

        private void ExplicitVoidMethodWithArgs(double arg)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitVoidMethodWithArgs)}({arg})");
        }

        private void ExplicitCallbackMethod(Callback callback)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitCallbackMethod)}()");
            callback(new object[] { Math.PI });
        }

        private void ExplicitCallbackMethodWithArgs(double arg, Callback callback)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitCallbackMethodWithArgs)}({arg})");
            callback(new object[] { Math.PI });
        }

        private void ExplicitPromiseMethod(Callback resolve, Callback reject)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitPromiseMethod)}()");

            try
            {
                resolve(new object[] { Math.PI });
            }
            catch (Exception ex)
            {
                reject(new object[] { "0x80000000", ex.Message, ex.StackTrace, ex });
            }
        }

        private void ExplicitPromiseMethodWithArgs(double arg, Callback resolve, Callback reject)
        {
            Debug.WriteLine($"{Name}.{nameof(ExplicitPromiseMethodWithArgs)}({arg})");

            try
            {
                resolve(new object[] { Math.PI });
            }
            catch (Exception ex)
            {
                reject(new object[] { "0x80000000", ex.Message, ex.StackTrace, ex });
            }
        }
    }
}
