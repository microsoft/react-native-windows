// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.IO;
using System.Threading.Tasks;
using Windows.Storage;

namespace ReactNative.Chakra.Executor
{
    /// <summary>
    /// Uses the native C++ interface to the JSRT for Chakra.
    /// </summary>
    public class NativeJavaScriptExecutor : IJavaScriptExecutor
    {
        private const string BytecodeFileName = "ReactNativeBundle.bin";

        private readonly ChakraBridge.NativeJavaScriptExecutor _executor;
        private readonly bool _useSerialization;

        /// <summary>
        /// Instantiates the <see cref="NativeJavaScriptExecutor"/>.
        /// </summary>
        public NativeJavaScriptExecutor()
            : this(false)
        {

        }

        /// <summary>
        /// Instantiates the <see cref="NativeJavaScriptExecutor"/>.
        /// </summary>
        /// <param name="useSerialization">true to use serialization, else false.</param>
        public NativeJavaScriptExecutor(bool useSerialization)
        {
            _executor = new ChakraBridge.NativeJavaScriptExecutor();
            Native.ThrowIfError((JavaScriptErrorCode)_executor.InitializeHost());
            _useSerialization = useSerialization;
        }

        /// <summary>
        /// Call the JavaScript method from the given module.
        /// </summary>
        /// <param name="moduleName">The module name.</param>
        /// <param name="methodName">The method name.</param>
        /// <param name="arguments">The arguments.</param>
        /// <returns>The flushed queue of native operations.</returns>
        public JToken CallFunctionReturnFlushedQueue(string moduleName, string methodName, JArray arguments)
        {
            if (moduleName == null)
                throw new ArgumentNullException(nameof(moduleName));
            if (methodName == null)
                throw new ArgumentNullException(nameof(methodName));
            if (arguments == null)
                throw new ArgumentNullException(nameof(arguments));

            var result = _executor.CallFunctionAndReturnFlushedQueue(moduleName, methodName, arguments.ToString(Formatting.None));
            Native.ThrowIfError((JavaScriptErrorCode)result.ErrorCode);

            return JToken.Parse(result.Result);
        }

        /// <summary>
        /// Disposes the <see cref="NativeJavaScriptExecutor"/> instance.
        /// </summary>
        public void Dispose()
        {
            Native.ThrowIfError((JavaScriptErrorCode)_executor.DisposeHost());
        }

        /// <summary>
        /// Flush the queue.
        /// </summary>
        /// <returns>The flushed queue of native operations.</returns>
        public JToken FlushedQueue()
        {
            var result = _executor.FlushedQueue();
            Native.ThrowIfError((JavaScriptErrorCode)result.ErrorCode);
            return JToken.Parse(result.Result);
        }

        /// <summary>
        /// Invoke the JavaScript callback.
        /// </summary>
        /// <param name="callbackId">The callback identifier.</param>
        /// <param name="arguments">The arguments.</param>
        /// <returns>The flushed queue of native operations.</returns>
        public JToken InvokeCallbackAndReturnFlushedQueue(int callbackId, JArray arguments)
        {
            if (arguments == null)
                throw new ArgumentNullException(nameof(arguments));

            var result = _executor.InvokeCallbackAndReturnFlushedQueue(callbackId, arguments.ToString(Formatting.None));
            Native.ThrowIfError((JavaScriptErrorCode)result.ErrorCode);
            return JToken.Parse(result.Result);
        }

        /// <summary>
        /// Runs the JavaScript at the given path.
        /// </summary>
        /// <param name="sourcePath">The source path.</param>
        /// <param name="sourceUrl">The source URL.</param>
        public void RunScript(string sourcePath, string sourceUrl)
        {
            if (sourcePath == null)
                throw new ArgumentNullException(nameof(sourcePath));
            if (sourceUrl == null)
                throw new ArgumentNullException(nameof(sourceUrl));

            try
            {
                var binPath = Path.Combine(ApplicationData.Current.LocalFolder.Path, BytecodeFileName);

                if (_useSerialization)
                {
                    var srcFileInfo = new FileInfo(sourcePath);
                    var binFileInfo = new FileInfo(binPath);

                    // The idea is to run the JS bundle and generate bytecode for it on a background thread.
                    // This eliminates the need to delay the first start when the app doesn't have bytecode.
                    // Next time the app starts, it checks if bytecode is still good and  runs it directly.
                    if (binFileInfo.Exists && binFileInfo.LastWriteTime > srcFileInfo.LastWriteTime)
                    {
                        Native.ThrowIfError((JavaScriptErrorCode)_executor.RunSerializedScript(sourcePath, binPath, sourceUrl));
                    }
                    else
                    {
                        Task.Run(() =>
                        {
                            try
                            {
                                // In Chakra JS engine, only one runtime can be active on a particular thread at a time,
                                // and a runtime can only be active on one thread at a time. However it's possible to
                                // create two runtimes and let them run on different threads.
                                var rt = new ChakraBridge.NativeJavaScriptExecutor();

                                Native.ThrowIfError((JavaScriptErrorCode)rt.InitializeHost());
                                Native.ThrowIfError((JavaScriptErrorCode)rt.SerializeScript(sourcePath, binPath));
                                Native.ThrowIfError((JavaScriptErrorCode)rt.DisposeHost());
                            }
                            catch (Exception)
                            {
                                // It's fine if the bytecode couldn't be generated: RN can still use the JS bundle.
                            }
                        });

                        Native.ThrowIfError((JavaScriptErrorCode)_executor.RunScript(sourcePath, sourceUrl));
                    }
                }
                else
                {
                    Native.ThrowIfError((JavaScriptErrorCode)_executor.RunScript(sourcePath, sourceUrl));
                }
            }
            catch (JavaScriptScriptException ex)
            {
                var jsonError = JavaScriptValueToJTokenConverter.Convert(ex.Error);
                var message = jsonError.Value<string>("message");
                var stackTrace = jsonError.Value<string>("stack");
                throw new Modules.Core.JavaScriptException(message ?? ex.Message, stackTrace, ex);
            }
        }

        public void SetCallSerializableNativeHook(CallSerializableNativeHook callSerializableNativeHook)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets a global variable in the JavaScript runtime.
        /// </summary>
        /// <param name="propertyName">The global variable name.</param>
        /// <param name="value">The value.</param>
        public void SetGlobalVariable(string propertyName, JToken value)
        {
            if (propertyName == null)
                throw new ArgumentNullException(nameof(propertyName));
            if (value == null)
                throw new ArgumentNullException(nameof(value));

            Native.ThrowIfError(
                (JavaScriptErrorCode)_executor.SetGlobalVariable(propertyName, value.ToString(Formatting.None)));
        }
    }
}
