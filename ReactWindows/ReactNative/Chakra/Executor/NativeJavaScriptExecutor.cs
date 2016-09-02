using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.IO;
using Windows.Storage;
using Windows.UI.Xaml;

namespace ReactNative.Chakra.Executor
{
    /// <summary>
    /// Uses the native C++ interface to the JSRT for Chakra.
    /// </summary>
    public class NativeJavaScriptExecutor : IJavaScriptExecutor
    {
        private readonly ChakraBridge.NativeJavaScriptExecutor _executor;
        private readonly bool _useSerialization;

        /// <summary>
        /// Instantiates the <see cref="NativeJavaScriptExecutor"/>.
        /// </summary>
        public NativeJavaScriptExecutor() : this(false)
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
        /// Runs the given script.
        /// </summary>
        /// <param name="script">The script.</param>
        /// <param name="sourceUrl">The source URL.</param>
        public void RunScript(string script, string sourceUrl)
        {
            if (script == null)
                throw new ArgumentNullException(nameof(script));
            if (sourceUrl == null)
                throw new ArgumentNullException(nameof(sourceUrl));

            try
            {
                if(_useSerialization)
                {
                    var binPath = Path.Combine(ApplicationData.Current.LocalFolder.Path, "ReactNativeBundle.bin");
                    Native.ThrowIfError((JavaScriptErrorCode)_executor.RunSerializedScript(script, binPath, sourceUrl));
                }
                else
                {
                    Native.ThrowIfError((JavaScriptErrorCode)_executor.RunScript(script, sourceUrl));
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
