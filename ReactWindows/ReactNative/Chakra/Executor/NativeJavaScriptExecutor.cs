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
    /// Tells how to serialize the JS bundle to bytecode when the app starts.
    /// </summary>
    public enum SerializationMode
    {
        /// <summary>
        /// Load the JS bundle without serialization.
        /// </summary>
        None,

        /// <summary>
        /// Serialize the bundle to bytecode first, then load the bundle.
        /// Next time the app starts it will load the existing bytecode.
        /// </summary>
        Foreground,

        /// <summary>
        /// Load the JS bundle and at the same time generate bytecode on
        /// a background thread. Next time the app starts, it will load the
        /// bytecode and won't need to create the 2nd thread.
        /// </summary>
        Background,
    }

    /// <summary>
    /// Uses the native C++ interface to the JSRT for Chakra.
    /// </summary>
    public class NativeJavaScriptExecutor : IJavaScriptExecutor
    {
        private const string BytecodeFileName = "ReactNativeBundle.bin";

        private readonly ChakraBridge.NativeJavaScriptExecutor _executor;
        private readonly SerializationMode _serializationMode;

        /// <summary>
        /// Instantiates the <see cref="NativeJavaScriptExecutor"/>.
        /// </summary>
        public NativeJavaScriptExecutor()
            : this(SerializationMode.None)
        {

        }

        /// <summary>
        /// Instantiates the <see cref="NativeJavaScriptExecutor"/>.
        /// </summary>
        /// <param name="serializationMode">true to use serialization, else false.</param>
        public NativeJavaScriptExecutor(SerializationMode serializationMode)
        {
            _executor = new ChakraBridge.NativeJavaScriptExecutor();
            Native.ThrowIfError((JavaScriptErrorCode)_executor.InitializeHost());
            _serializationMode = serializationMode;
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

                switch (_serializationMode)
                {
                    case SerializationMode.Foreground:
                        Native.ThrowIfError((JavaScriptErrorCode)_executor.RunSerializedScript(sourcePath, binPath, sourceUrl));
                        break;

                    case SerializationMode.Background:
                        var binFileInfo = new FileInfo(binPath);

                        if (binFileInfo.Exists)
                        {
                            // This function checks if the bytecode file matches the JS bundle and makes
                            // a new bytecode file if necessary, but it does this on the same thread.
                            Native.ThrowIfError((JavaScriptErrorCode)_executor.RunSerializedScript(sourcePath, binPath, sourceUrl));
                        }
                        else
                        {
                            // If the bytecode file doesn't exist yet, create it on a background thread,
                            // while loading the JS bundle on the current thread.
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

                        break;

                    case SerializationMode.None:
                        Native.ThrowIfError((JavaScriptErrorCode)_executor.RunScript(sourcePath, sourceUrl));
                        break;

                    default:
                        throw new Exception("Invalid serialization mode: " + _serializationMode);
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
