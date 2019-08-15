// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"
#include "CallSyncHandler.h"
#include "ChakraHost.h"
#include "ChakraStringResult.h"
#include "FlushQueueImmediateHandler.h"

using namespace Platform;

namespace ChakraBridge {

public
enum class LogLevel { Trace = 0, Info, Warning, Error };

inline LogLevel LogLevelToEnum(int loglevel) {
  switch (loglevel) {
    case 0:
      return LogLevel::Trace;
    case 1:
      return LogLevel::Info;
    case 2:
      return LogLevel::Warning;
    case 3:
      return LogLevel::Error;
    default:
      return LogLevel::Info;
  }
}

public
delegate void NewLogLine(LogLevel logLevel, Platform::String ^ logline);

/// <summary>
/// This class interfaces with the unmanaged JSRT Chakra Host.
/// </summary>
public
ref class NativeJavaScriptExecutor sealed {
 public:
  /// <summary>
  /// Initializes the JSRT session.
  /// </summary>
  /// <returns>
  /// JsNoError is no error, else a JsErrorCode with the appropriate error.
  /// </returns>
  int InitializeHost();

  /// <summary>
  /// Disposes the current JSRT session.
  /// </summary>
  /// <returns>
  /// JsNoError is no error, else a JsErrorCode with the appropriate error.
  /// </returns>
  int DisposeHost();

  /// <summary>
  /// Gets the global variable value.
  /// </summary>
  /// <param name="variableName">The global variable to get.</param>
  /// <returns>
  /// A compount result with the JSON stringified value and an error code if any
  /// occurred.
  /// </returns>
  ChakraStringResult GetGlobalVariable(String ^ variableName);

  /// <summary>
  /// Sets the global variable
  /// </summary>
  /// <param name="variableName">The variable name to set.</param>
  /// <param name="value">The value to set on the global variable.</param>
  /// <returns>
  /// JsNoError is no error, else a JsErrorCode with the appropriate error.
  /// </returns>
  int SetGlobalVariable(String ^ variableName, String ^ value);

  /// <summary>
  /// Runs the given script with the source URI and returns the result.
  /// </summary>
  /// <param name="source">The source of the script to run.</param>
  /// <param name="sourceUri">The source URI of the script to run.</param>
  /// <returns>
  /// A compount result with the JSON stringified value and an error code if any
  /// occurred.
  /// </returns>
  int RunScript(String ^ source, String ^ sourceUri);

  int SerializeScript(String ^ source, String ^ serialized);

  /// <summary>
  /// Runs the given serialized script with the source URI and returns the
  /// result.
  /// </summary>
  /// <param name="source">The source of the script to run.</param>
  /// <param name="serialized">The serialized script to run.</param>
  /// <param name="sourceUri">The source URI of the script to run.</param>
  /// <returns>
  /// A compount result with the JSON stringified value and an error code if any
  /// occurred.
  /// </returns>
  int RunSerializedScript(
      String ^ source,
      String ^ serialized,
      String ^ sourceUri);

  /// <summary>
  /// Calls the underlying function with the given module and method name and
  /// JSON stringified arguments.
  /// </summary>
  /// <param name="moduleName">The module name to call.</param>
  /// <param name="methodName">The method name on the module to call.</param>
  /// <param name="args">JSON stringified arguments to call on the
  /// module.</param> <returns> A compount result with the JSON stringified
  /// value and an error code if any occurred.
  /// </returns>
  ChakraStringResult CallFunctionAndReturnFlushedQueue(
      String ^ moduleName,
      String ^ methodName,
      String ^ args);

  /// <summary>
  /// Calls the underlying function with the callback ID and JSON stringified
  /// arguments.
  /// </summary>
  /// <param name="callbackId">The callback ID.</param>
  /// <param name="args">JSON stringified arguments to call on the
  /// module.</param> <returns> A compount result with the JSON stringified
  /// value and an error code if any occurred.
  /// </returns>
  ChakraStringResult InvokeCallbackAndReturnFlushedQueue(
      int callbackId,
      String ^ args);

  /// <summary>
  /// Calls the flush queue function.
  /// </summary>
  /// <returns>
  /// A compount result with the JSON stringified value and an error code if any
  /// occurred.
  /// </returns>
  ChakraStringResult FlushedQueue();

  /// <summary>
  /// Event to provide a callback for logging
  /// </summary>
  static event NewLogLine ^ OnNewLogLine {
    Windows::Foundation::EventRegistrationToken add(NewLogLine ^ handler) {
      onNewLogLineAssigned = true;

      g_loggingCallback = [=](int level, const wchar_t *logline) {
        if (onNewLogLineAssigned) {
          onNewLogLine(
              LogLevelToEnum(level), ref new Platform::String(logline));
        }
#ifdef _DEBUG
        else {
          OutputDebugStringW(LogLevelToString(level));
          OutputDebugStringW(logline);
          OutputDebugStringW(L"\n");
        }
#endif
      };
      g_isLoggingEnabled = true;

      return onNewLogLine += handler;
    }
    void remove(Windows::Foundation::EventRegistrationToken token) {
      onNewLogLine -= token;
    }
    void raise(LogLevel logLevel, Platform::String ^ logLine) {
      return onNewLogLine(logLevel, logLine);
    }
  };

  /// <summary>
  /// Sets the flush queue immediately hook
  /// </summary>
  void SetFlushQueueImmediate(FlushQueueImmediateHandler ^ handler);

  /// <summary>
  /// Sets the sync native method hook.
  /// </summary>
  void SetCallSyncHook(CallSyncHandler ^ handler);

 private:
  JsErrorCode CheckAndGetMethodRef(
      const wchar_t *methodName,
      JsValueRef &value);
  void SafeReleaseJsValueRef(JsValueRef &value);

  ChakraHost host;
  static event NewLogLine ^ onNewLogLine;
  static bool onNewLogLineAssigned;
  JsValueRef callFunctionMethod;
  JsValueRef invokeCallbackMethod;
  JsValueRef flushQueueMethod;
};

bool NativeJavaScriptExecutor::onNewLogLineAssigned = false;

}; // namespace ChakraBridge
