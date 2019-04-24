// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Tracing;
using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using static System.FormattableString;

namespace ReactNative.Chakra.Executor
{
    /// <summary>
    /// JavaScript runtime wrapper.
    /// </summary>
    public sealed class ChakraJavaScriptExecutor : IJavaScriptExecutor
    {
        private const string MagicFileName = "UNBUNDLE";
        private const uint MagicFileHeader = 0xFB0BD1E5;

        private const string JsonName = "JSON";
        private const string FBBatchedBridgeVariableName = "__fbBatchedBridge";

        private readonly JavaScriptRuntime _runtime;

        private IJavaScriptUnbundle _unbundle;

        private JavaScriptSourceContext _context;

        private JavaScriptNativeFunction _nativeLoggingHook;
        private JavaScriptNativeFunction _nativeRequire;
        private JavaScriptNativeFunction _nativeCallSyncHook;
        private JavaScriptNativeFunction _nativeFlushQueueImmediate;

        private JavaScriptValue _globalObject;

        private JavaScriptValue _callFunctionAndReturnFlushedQueueFunction;
        private JavaScriptValue _invokeCallbackAndReturnFlushedQueueFunction;
        private JavaScriptValue _flushedQueueFunction;
        private JavaScriptValue _fbBatchedBridge;

#if !NATIVE_JSON_MARSHALING
        private JavaScriptValue _parseFunction;
        private JavaScriptValue _stringifyFunction;
#endif

        private Func<int, int, JArray, JToken> _callSyncHook;
        private Action<JToken> _flushQueueImmediate;

        /// <summary>
        /// Instantiates the <see cref="ChakraJavaScriptExecutor"/>.
        /// </summary>
        public ChakraJavaScriptExecutor()
        {
            _runtime = JavaScriptRuntime.Create();
            _context = JavaScriptSourceContext.FromIntPtr(IntPtr.Zero);
            InitializeChakra();
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

            var moduleNameValue = JavaScriptValue.FromString(moduleName);
            moduleNameValue.AddRef();
            var methodNameValue = JavaScriptValue.FromString(methodName);
            methodNameValue.AddRef();
            var argumentsValue = ConvertJson(arguments);
            argumentsValue.AddRef();

            var callArguments = new JavaScriptValue[4];
            callArguments[0] = EnsureGlobalObject();
            callArguments[1] = moduleNameValue;
            callArguments[2] = methodNameValue;
            callArguments[3] = argumentsValue;
            var method = EnsureCallFunction();
            var flushedQueue = ConvertJson(method.CallFunction(callArguments));

            argumentsValue.Release();
            methodNameValue.Release();
            moduleNameValue.Release();

            return flushedQueue;
        }

        /// <summary>
        /// Flush the queue.
        /// </summary>
        /// <returns>The flushed queue of native operations.</returns>
        public JToken FlushedQueue()
        {
            var method = EnsureFlushedQueueFunction();
            var callArguments = new JavaScriptValue[1];
            callArguments[0] = EnsureGlobalObject();
            return ConvertJson(method.CallFunction(callArguments));
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

            var callbackIdValue = JavaScriptValue.FromInt32(callbackId);
            callbackIdValue.AddRef();
            var argumentsValue = ConvertJson(arguments);
            argumentsValue.AddRef();

            var callArguments = new JavaScriptValue[3];
            callArguments[0] = EnsureGlobalObject();
            callArguments[1] = callbackIdValue;
            callArguments[2] = argumentsValue;
            var method = EnsureInvokeFunction();
            var flushedQueue = ConvertJson(method.CallFunction(callArguments));

            argumentsValue.Release();
            callbackIdValue.Release();

            return flushedQueue;
        }

        /// <summary>
        /// Runs the script at the given path.
        /// </summary>
        /// <param name="sourcePath">The source path.</param>
        /// <param name="sourceUrl">The source URL.</param>
        public void RunScript(string sourcePath, string sourceUrl)
        {
            if (sourcePath == null)
                throw new ArgumentNullException(nameof(sourcePath));
            if (sourceUrl == null)
                throw new ArgumentNullException(nameof(sourceUrl));

            var startupCode = default(string);
            if (IsUnbundle(sourcePath))
            {
                _unbundle = new FileBasedJavaScriptUnbundle(sourcePath);
                InstallNativeRequire();
                startupCode = _unbundle.GetStartupCode();
            }
            else if (IsIndexedUnbundle(sourcePath))
            {
                _unbundle = new IndexedJavaScriptUnbundle(sourcePath);
                InstallNativeRequire();
                startupCode = _unbundle.GetStartupCode();
            }
            else
            {
                startupCode = LoadScript(sourcePath);
            }

            EvaluateScript(startupCode, sourceUrl);
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

            var javaScriptValue = ConvertJson(value);
            var propertyId = JavaScriptPropertyId.FromString(propertyName);
            EnsureGlobalObject().SetProperty(propertyId, javaScriptValue, true);
        }

        /// <summary>
        /// Gets a global variable from the JavaScript runtime.
        /// </summary>
        /// <param name="propertyName">The global variable name.</param>
        /// <returns>The value.</returns>
        public JToken GetGlobalVariable(string propertyName)
        {
            if (propertyName == null)
                throw new ArgumentNullException(nameof(propertyName));

            var propertyId = JavaScriptPropertyId.FromString(propertyName);
            return ConvertJson(EnsureGlobalObject().GetProperty(propertyId));
        }

        /// <summary>
        /// Sets a callback for synchronous native methods.
        /// </summary>
        /// <param name="callSyncHook">The sync hook for native methods.</param>
        public void SetCallSyncHook(Func<int, int, JArray, JToken> callSyncHook)
        {
            _callSyncHook = callSyncHook;
        }

        /// <summary>
        /// Sets a callback for immediate queue flushes.
        /// </summary>
        /// <param name="flushQueueImmediate">The callback.</param>
        public void SetFlushQueueImmediate(Action<JToken> flushQueueImmediate)
        {
            _flushQueueImmediate = flushQueueImmediate;
        }

        /// <summary>
        /// Disposes the <see cref="ChakraJavaScriptExecutor"/> instance.
        /// </summary>
        public void Dispose()
        {
            JavaScriptContext.Current = JavaScriptContext.Invalid;
            _runtime.Dispose();
            _unbundle?.Dispose();
        }

        private void InitializeChakra()
        {
            JavaScriptContext.Current = _runtime.CreateContext();

            _nativeLoggingHook = NativeLoggingHook;
            EnsureGlobalObject().SetProperty(
                JavaScriptPropertyId.FromString("nativeLoggingHook"),
                JavaScriptValue.CreateFunction(_nativeLoggingHook),
                true);

            _nativeCallSyncHook = NativeCallSyncHook;
            EnsureGlobalObject().SetProperty(
                JavaScriptPropertyId.FromString("nativeCallSyncHook"),
                JavaScriptValue.CreateFunction(_nativeCallSyncHook),
                true);

            _nativeFlushQueueImmediate = NativeFlushQueueImmediate;
            EnsureGlobalObject().SetProperty(
                JavaScriptPropertyId.FromString("nativeFlushQueueImmediate"),
                JavaScriptValue.CreateFunction(_nativeFlushQueueImmediate),
                true);
        }

        private void EvaluateScript(string script, string sourceUrl)
        {
            try
            {
                _context = JavaScriptSourceContext.Increment(_context);
                JavaScriptContext.RunScript(script, _context, sourceUrl);
            }
            catch (JavaScriptScriptException ex)
            {
                var jsonError = JavaScriptValueToJTokenConverter.Convert(ex.Error);
                var message = jsonError.Value<string>("message");
                var stackTrace = jsonError.Value<string>("stack");
                throw new Modules.Core.JavaScriptException(message ?? ex.Message, stackTrace, ex);
            }
        }

        #region JSON Marshaling

#if NATIVE_JSON_MARSHALING
        private JavaScriptValue ConvertJson(JToken token)
        {
            return JTokenToJavaScriptValueConverter.Convert(token);
        }

        private JToken ConvertJson(JavaScriptValue value)
        {
            return JavaScriptValueToJTokenConverter.Convert(value);
        }
#else
        private JavaScriptValue ConvertJson(JToken token)
        {
            var jsonString = token?.ToString(Formatting.None) ?? "null";
            return ConvertJson(jsonString);
        }

        private JavaScriptValue ConvertJson(string jsonString)
        {
            var jsonStringValue = JavaScriptValue.FromString(jsonString);
            jsonStringValue.AddRef();
            var parseFunction = EnsureParseFunction();
            var jsonValue = parseFunction.CallFunction(_globalObject, jsonStringValue);
            jsonStringValue.Release();
            return jsonValue;
        }

        private JToken ConvertJson(JavaScriptValue value)
        {
            var stringifyFunction = EnsureStringifyFunction();
            var jsonStringValue = stringifyFunction.CallFunction(_globalObject, value);
            jsonStringValue.AddRef();
            var jsonString = jsonStringValue.ToString();
            jsonStringValue.Release();
            return JToken.Parse(jsonString);
        }
#endif

        #endregion

        #region Console Callbacks
        private JavaScriptValue NativeLoggingHook(
            JavaScriptValue callee,
            bool isConstructCall,
            JavaScriptValue[] arguments,
            ushort argumentCount,
            IntPtr callbackData)
        {
            try
            {
                var message = arguments[1].ToString();
                var logLevel = (LogLevel)(int)arguments[2].ToDouble();
                RnLog.Info("JS", $"{logLevel} {message}");
            }
            catch
            {
                RnLog.Error("JS", $"Unable to process JavaScript console statement");
            }

            return JavaScriptValue.Undefined;
        }
        #endregion

        #region Native Flush Queue Immediate Hook
        private JavaScriptValue NativeFlushQueueImmediate(
            JavaScriptValue callee,
            bool isConstructCall,
            JavaScriptValue[] arguments,
            ushort argumentCount,
            IntPtr callbackData)
        {
            if (argumentCount != 2)
            {
                throw new ArgumentOutOfRangeException(nameof(argumentCount), "Expected exactly two arguments (global, flushedQueue)");
            }

            if (_flushQueueImmediate == null)
            {
                throw new InvalidOperationException("Callback hook for `nativeFlushQueueImmediate` has not been set.");
            }

            _flushQueueImmediate(ConvertJson(arguments[1]));

            return JavaScriptValue.Undefined;
        }
        #endregion

        #region Native Call Sync Hook
        private JavaScriptValue NativeCallSyncHook(
            JavaScriptValue callee,
            bool isConstructCall,
            JavaScriptValue[] arguments,
            ushort argumentCount,
            IntPtr callbackData)
        {
            if (argumentCount != 4)
            {
                throw new ArgumentOutOfRangeException(nameof(argumentCount), "Expected exactly four arguments (global, moduleId, methodId, and args).");
            }

            if (_callSyncHook == null)
            {
                throw new InvalidOperationException("Sync hook has not been set.");
            }

            var moduleId = (int)arguments[1].ToDouble();
            var methodId = (int)arguments[2].ToDouble();
            var args = (JArray)ConvertJson(arguments[3]);
            
            try
            {
                var result = _callSyncHook(moduleId, methodId, args);
                return ConvertJson(result);
            }
            catch (Exception e)
            {
                var error = JavaScriptValue.CreateError(JavaScriptValue.FromString(e.Message));
                Native.JsSetException(error);
                return JavaScriptValue.Invalid;
            }
        }
        #endregion

        #region Native Require
        private void InstallNativeRequire()
        {
            _nativeRequire = NativeRequire;
            EnsureGlobalObject().SetProperty(
                JavaScriptPropertyId.FromString("nativeRequire"),
                JavaScriptValue.CreateFunction(_nativeRequire),
                true);
        }

        private JavaScriptValue NativeRequire(
            JavaScriptValue callee,
            bool isConstructCall,
            JavaScriptValue[] arguments,
            ushort argumentCount,
            IntPtr callbackData)
        {
            if (argumentCount != 2)
            {
                throw new ArgumentOutOfRangeException(nameof(argumentCount), "Expected exactly two arguments (global and moduleId).");
            }

            var moduleId = arguments[1].ToDouble();
            if (moduleId <= 0)
            {
                throw new ArgumentOutOfRangeException(
                    nameof(arguments),
                    Invariant($"Received invalid module ID '{moduleId}'."));
            }

            var module = _unbundle.GetModule((int)moduleId);
            EvaluateScript(module.Source, module.SourceUrl);
            return JavaScriptValue.Invalid;
        }
        #endregion

        #region Global Helpers

        private JavaScriptValue EnsureGlobalObject()
        {
            if (!_globalObject.IsValid)
            {
                _globalObject = JavaScriptValue.GlobalObject;
            }

            return _globalObject;
        }

        private JavaScriptValue EnsureParseFunction()
        {
            if (!_parseFunction.IsValid)
            {
                var globalObject = EnsureGlobalObject();
                var jsonObject = globalObject.GetProperty(JavaScriptPropertyId.FromString(JsonName));
                _parseFunction = jsonObject.GetProperty(JavaScriptPropertyId.FromString("parse"));
            }

            return _parseFunction;
        }

        private JavaScriptValue EnsureBatchedBridge()
        {
            if (!_fbBatchedBridge.IsValid)
            {
                var globalObject = EnsureGlobalObject();
                var propertyId = JavaScriptPropertyId.FromString(FBBatchedBridgeVariableName);
                var fbBatchedBridge = globalObject.GetProperty(propertyId);
                if (fbBatchedBridge.ValueType != JavaScriptValueType.Object)
                {
                    throw new InvalidOperationException(
                        Invariant($"Could not resolve '{FBBatchedBridgeVariableName}' object.  Check the JavaScript bundle to ensure it is generated correctly."));
                }

                _fbBatchedBridge = fbBatchedBridge;
            }

            return _fbBatchedBridge;
        }

        private JavaScriptValue EnsureStringifyFunction()
        {
            if (!_stringifyFunction.IsValid)
            {
                var globalObject = EnsureGlobalObject();
                var jsonObject = globalObject.GetProperty(JavaScriptPropertyId.FromString(JsonName));
                _stringifyFunction = jsonObject.GetProperty(JavaScriptPropertyId.FromString("stringify"));
            }

            return _stringifyFunction;
        }

        private JavaScriptValue EnsureCallFunction()
        {
            if (!_callFunctionAndReturnFlushedQueueFunction.IsValid)
            {
                var fbBatchedBridge = EnsureBatchedBridge();
                var functionPropertyId = JavaScriptPropertyId.FromString("callFunctionReturnFlushedQueue");
                _callFunctionAndReturnFlushedQueueFunction = fbBatchedBridge.GetProperty(functionPropertyId);
            }

            return _callFunctionAndReturnFlushedQueueFunction;
        }

        private JavaScriptValue EnsureInvokeFunction()
        {
            if (!_invokeCallbackAndReturnFlushedQueueFunction.IsValid)
            {
                var fbBatchedBridge = EnsureBatchedBridge();
                var functionPropertyId = JavaScriptPropertyId.FromString("invokeCallbackAndReturnFlushedQueue");
                _invokeCallbackAndReturnFlushedQueueFunction = fbBatchedBridge.GetProperty(functionPropertyId);
            }

            return _invokeCallbackAndReturnFlushedQueueFunction;
        }

        private JavaScriptValue EnsureFlushedQueueFunction()
        {
            if (!_flushedQueueFunction.IsValid)
            {
                var fbBatchedBridge = EnsureBatchedBridge();
                var functionPropertyId = JavaScriptPropertyId.FromString("flushedQueue");
                _flushedQueueFunction = fbBatchedBridge.GetProperty(functionPropertyId);
            }

            return _flushedQueueFunction;
        }

        #endregion

        #region File IO
        private static string LoadScript(string fileName)
        {
            try
            {
                return File.ReadAllText(fileName);
            }
            catch (Exception ex)
            {
                var exceptionMessage = Invariant($"File read exception for asset '{fileName}'.");
                throw new InvalidOperationException(exceptionMessage, ex);
            }
        }
        #endregion

        #region Unbundle
        class JavaScriptUnbundleModule
        {
            public JavaScriptUnbundleModule(string source, string sourceUrl)
            {
                SourceUrl = sourceUrl;
                Source = source;
            }

            public string SourceUrl { get; }

            public string Source { get; }
        }

        interface IJavaScriptUnbundle : IDisposable
        {
            JavaScriptUnbundleModule GetModule(int index);

            string GetStartupCode();
        }

        class FileBasedJavaScriptUnbundle : IJavaScriptUnbundle
        {
            private readonly string _sourcePath;
            private readonly string _modulesPath;

            public FileBasedJavaScriptUnbundle(string sourcePath)
            {
                _sourcePath = sourcePath;
                _modulesPath = GetUnbundleModulesDirectory(sourcePath);
            }

            public JavaScriptUnbundleModule GetModule(int index)
            {
                var sourceUrl = index + ".js";
                var fileName = Path.Combine(_modulesPath, sourceUrl);
                var source = LoadScript(fileName);
                return new JavaScriptUnbundleModule(source, sourceUrl);
            }

            public string GetStartupCode()
            {
                return LoadScript(_sourcePath);
            }

            public void Dispose()
            {
            }
        }

        class IndexedJavaScriptUnbundle : IJavaScriptUnbundle
        {
            private const int HeaderSize = 12;

            private readonly string _sourcePath;

            private Stream _stream;
            private byte[] _moduleTable;
            private int _baseOffset;

            public IndexedJavaScriptUnbundle(string sourcePath)
            {
                _sourcePath = sourcePath;
            }

            public JavaScriptUnbundleModule GetModule(int index)
            {
                var offset = InetHelpers.LittleEndianToHost(BitConverter.ToUInt32(_moduleTable, index * 8));
                var length = InetHelpers.LittleEndianToHost(BitConverter.ToUInt32(_moduleTable, index * 8 + 4));
                _stream.Seek(_baseOffset + offset, SeekOrigin.Begin);
                var moduleData = new byte[length];
                if (_stream.Read(moduleData, 0, (int) length) < length)
                {
                    throw new InvalidOperationException("Reached end of file before end of unbundle module.");
                }

                var source = Encoding.UTF8.GetString(moduleData);
                var sourceUrl = index + ".js";
                return new JavaScriptUnbundleModule(source, sourceUrl);
            }

            public string GetStartupCode()
            {
                _stream = File.OpenRead(_sourcePath);
                var header = new byte[HeaderSize];
                if (_stream.Read(header, 0, HeaderSize) < HeaderSize)
                {
                    throw new InvalidOperationException("Reached end of file before end of indexed unbundle header.");
                }

                var numberOfTableEntries = InetHelpers.LittleEndianToHost(BitConverter.ToUInt32(header, 4));
                var startupCodeSize = InetHelpers.LittleEndianToHost(BitConverter.ToUInt32(header, 8));
                var moduleTableSize = numberOfTableEntries * 8 /* bytes per entry */;
                _baseOffset = HeaderSize + (int)moduleTableSize;
                _moduleTable = new byte[moduleTableSize];
                if (_stream.Read(_moduleTable, 0, (int)moduleTableSize) < moduleTableSize)
                {
                    throw new InvalidOperationException("Reached end of file before end of indexed unbundle module table.");
                }

                var startupCodeBuffer = new byte[startupCodeSize];
                if (_stream.Read(startupCodeBuffer, 0, (int)startupCodeSize) < startupCodeSize)
                {
                    throw new InvalidOperationException("Reached end of file before end of startup code.");
                }

                return Encoding.UTF8.GetString(startupCodeBuffer);
            }

            public void Dispose()
            {
                _stream.Dispose();
            }
        }

        private static bool IsUnbundle(string sourcePath)
        {
            var magicFilePath = Path.Combine(GetUnbundleModulesDirectory(sourcePath), MagicFileName);
            if (!File.Exists(magicFilePath))
            {
                return false;
            }

            using (var stream = File.OpenRead(magicFilePath))
            {
                var header = new byte[4];
                var read = stream.Read(header, 0, 4);
                if (read < 4)
                {
                    return false;
                }

                var magicHeader = BitConverter.ToUInt32(header, 0);
                return InetHelpers.LittleEndianToHost(magicHeader) == MagicFileHeader;
            }
        }

        private static bool IsIndexedUnbundle(string sourcePath)
        {
            using (var stream = File.OpenRead(sourcePath))
            {
                var header = new byte[4];
                var read = stream.Read(header, 0, 4);
                if (read < 4)
                {
                    return false;
                }

                var magic = InetHelpers.LittleEndianToHost(BitConverter.ToUInt32(header, 0));
                return magic == MagicFileHeader;
            }
        }

        private static string GetUnbundleModulesDirectory(string sourcePath)
        {
            return Path.Combine(Path.GetDirectoryName(sourcePath), "js-modules");
        }
        #endregion
    }
}
