// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ChakraBridge;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Tracing;
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
        /// Initializes the hooking of JS logging to RnLog.
        /// </summary>
        static public void StartRnLogging()
        {
            ChakraBridge.NativeJavaScriptExecutor.OnNewLogLine += JsExecutorOnNewLogLine;
        }

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

        private static void JsExecutorOnNewLogLine(ChakraBridge.LogLevel logLevel, string logline)
        {
            // ChakraBridge.LogLevel value is lost when it gets marshaled to .Net native optimized code,
            // we need to do a proper marshaling to get actual value
            // Also, JavaScript already has a log level in the message, hence just RnLog.Info
            string tag = "JS";
            FormattableString message = $"{logline}";

            switch (logLevel)
            {
                case ChakraBridge.LogLevel.Error:
                    RnLog.Error(tag, message);
                    break;
                case ChakraBridge.LogLevel.Warning:
                    RnLog.Warn(tag, message);
                    break;
                case ChakraBridge.LogLevel.Info:
                case ChakraBridge.LogLevel.Trace:
                    RnLog.Info(tag, message);
                    break;
            }
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

                    bool ranSuccessfully = false;
                    // The idea is to run the JS bundle and generate bytecode for it on a background thread.
                    // This eliminates the need to delay the first start when the app doesn't have bytecode.
                    // Next time the app starts, it checks if bytecode is still good and  runs it directly.
                    if (binFileInfo.Exists && binFileInfo.LastWriteTime > srcFileInfo.LastWriteTime)
                    {
                        try
                        {
                            Native.ThrowIfError((JavaScriptErrorCode)_executor.RunSerializedScript(sourcePath, binPath, sourceUrl));
                            ranSuccessfully = true;
                        }
                        catch (JavaScriptUsageException exc)
                        {
                            if (exc.ErrorCode == JavaScriptErrorCode.BadSerializedScript)
                            {
                                // Bytecode format is dependent on Chakra engine version, so an OS upgrade may require a recompilation
                                RnLog.Warn(ReactConstants.RNW, $"Serialized bytecode script is corrupted or wrong format, will generate new one");
                            }
                            else
                            {
                                // Some more severe error. We still have a chance (recompiling), so we keep continuing.
                                RnLog.Error(ReactConstants.RNW, exc, $"Failed to run serialized bytecode script, will generate new one");
                            }

                            File.Delete(binPath);
                        }
                    }
                    else
                    {
                        RnLog.Info(ReactConstants.RNW, $"Serialized bytecode script doesn't exist or is obsolete, will generate one");
                    }

                    if (!ranSuccessfully)
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
                            catch (Exception ex)
                            {
                                RnLog.Error(ReactConstants.RNW, ex, $"Failed to generate serialized bytecode script.");
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

        /// <summary>
        /// Set a callback for flushing the queue immediately.
        /// </summary>
        /// <param name="flushQueueImmediate">The callback.</param>
        public void SetFlushQueueImmediate(Action<JToken> flushQueueImmediate)
        {
            if (flushQueueImmediate == null)
                throw new ArgumentNullException(nameof(flushQueueImmediate));

            _executor.SetFlushQueueImmediate(args =>
                flushQueueImmediate(JToken.Parse(args)));
        }

        /// <summary>
        /// Sets a callback for synchronous native methods.
        /// </summary>
        /// <param name="callSyncHook">The sync hook for native methods.</param>
        public void SetCallSyncHook(Func<int, int, JArray, JToken> callSyncHook)
        {
            if (callSyncHook == null)
                throw new ArgumentNullException(nameof(callSyncHook));

            _executor.SetCallSyncHook((moduleId, methodId, args) =>
            {
                try
                {
                    var result = callSyncHook(moduleId, methodId, JArray.Parse(args))?.ToString(Formatting.None) ?? "null";
                    return new ChakraStringResult
                    {
                        Result = result,
                    };
                }
                catch (Exception e)
                {
                    return new ChakraStringResult
                    {
                        Result = e.Message,
                        ErrorCode = -1 /* Only has to be non-zero */,
                    };
                }
            });
        }

        /// <summary>
        /// Sets a global variable in the JavaScript runtime.
        /// </summary>
        /// <param name="propertyName">The global variable name.</param>
        /// <param name="value">The value.</param>
        public void SetGlobalVariable(string propertyName, string value)
        {
            if (propertyName == null)
                throw new ArgumentNullException(nameof(propertyName));
            if (value == null)
                throw new ArgumentNullException(nameof(value));

            Native.ThrowIfError(
                (JavaScriptErrorCode)_executor.SetGlobalVariable(propertyName, value));
        }
    }
}
