// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"
#include "JsModulesUnbundle.h"
#include "CallSyncHandler.h"
#include "FlushQueueImmediateHandler.h"
#include <jsrt.h>

bool CompareLastWrite(const wchar_t* szPath1, const wchar_t* szPath2);

/// <summary>
/// This class wraps the main functionality dealing with the JSRT.
/// </summary>
class ChakraHost
{
public:
	/// <summary>
	/// Loads a memory mapped file from the given path.
	/// </summary>
	/// <param name="szPath">The location of the file to load.</param>
	/// <param name="pData">The location to write the mapped file data.</param>
	/// <param name="hFile">The file handle.</param>
	/// <param name="hFile">The mapped file handle.</param>
	/// <param name="bIsReadOnly">Signals that the mapped file is only for reading.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	static JsErrorCode LoadByteCode(const wchar_t* szPath, BYTE** pData, HANDLE* hFile, HANDLE* hMap, bool bIsReadOnly);

	/// <summary>
	/// Loads the file contents from the given path.
	/// </summary>
	/// <param name="szPath">The location of the file to load.</param>
	/// <param name="pszData">The location to write the file contents.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	static JsErrorCode LoadFileContents(const wchar_t* szPath, wchar_t** pszData);

    /// <summary>
    /// Initializes a JSRT session with the context, globals, JSON, and console logging.
    /// </summary>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode Init();

    /// <summary>
    /// Destorys the current JSRT session.
    /// </summary>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode Destroy();

    /// <summary>
    /// Runs the script from the file location with the source URI and returns the <see cref="JsValueRef" /> result.
    /// </summary>
    /// <param name="szPath">The location of the script to run.</param>
    /// <param name="szSourceUri">The source URI of the script.</param>
    /// <param name="result">[Out] The result from running the script.</param>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode RunScript(const wchar_t* szPath, const wchar_t* szSourceUri, JsValueRef* result);

    /// <summary>
    /// Serializes the script and writes the bytecode to the specified file.
    /// </summary>
    JsErrorCode SerializeScript(const wchar_t* szPath, const wchar_t* szSerializedPath);

    /// <summary>
    /// Runs the serialized script from the serialized file location with source file path, source URI and returns the <see cref="JsValueRef" /> result.
    /// </summary>
    /// <param name="szPath">The location of the script to run.</param>
    /// <param name="szSerializedPath">The location of the serialized script to run.</param>
    /// <param name="szSourceUri">The source URI of the script.</param>
    /// <param name="result">[Out] The result from running the script.</param>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode RunSerializedScript(const wchar_t* szPath, const wchar_t* szSerializedPath, const wchar_t* szSourceUri, JsValueRef* result);

    /// <summary>
    /// Calls JSON stringify on the given <see cref="JsValueRef" /> and returns the <see cref="JsValueRef" /> result.
    /// </summary>
    /// <param name="argument">The argument to stringify.</param>
    /// <param name="result">[Out] The result from running JSON.stringify.</param>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode JsonStringify(JsValueRef argument, JsValueRef* result);

    /// <summary>
    /// Calls JSON parse on the given <see cref="JsValueRef" /> and returns the <see cref="JsValueRef" /> result.
    /// </summary>
    /// <param name="argument">The argument to parse.</param>
    /// <param name="result">[Out] The result from running JSON.parse.</param>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode JsonParse(JsValueRef argument, JsValueRef* result);

    /// <summary>
    /// Gets a global variable and returns the <see cref="JsValueRef" /> result.
    /// </summary>
    /// <param name="szPropertyName">The property name from global to get.</param>
    /// <param name="result">[Out] The global property value.</param>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode GetGlobalVariable(const wchar_t* szPropertyName, JsValueRef* result);

    /// <summary>
    /// Sets a global variable with the <see cref="JsValueRef" />.
    /// </summary>
    /// <param name="szPropertyName">The property name from global to get.</param>
    /// <param name="value">The global property value to set.</param>
    /// <returns>
    /// JsNoError is no error, else a JsErrorCode with the appropriate error.
    /// </returns>
    JsErrorCode SetGlobalVariable(const wchar_t* szPropertyName, JsValueRef value);

	/// <summary>
	/// Evaluates the given script.
	/// </summary>
	/// <param name="szScript">The script.</param>
	/// <param name="szSourceUri">The source URI.</param>
	/// <param name="result">The return value.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode EvaluateScript(const wchar_t* szScript, const wchar_t* szSourceUri, JsValueRef* result);

    /// <summary>
    /// Sets a callback for sync native methods.
    /// </summary>
    /// <param name="handler">The call sync hook.</param>
    void SetCallSyncHook(ChakraBridge::CallSyncHandler^ handler);

    /// <summary>
    /// Sets a callback for flushing the queue immediately.
    /// </summary>
    /// <param name="handler">The callback.</param>
    void SetFlushQueueImmediate(ChakraBridge::FlushQueueImmediateHandler^ handler);

    /// <summary>
    /// The JSRT global object for the session.
    /// </summary>
    JsValueRef globalObject;

	/// <summary>
	/// The reference to the unbundle manager.
	/// </summary>
	JsModulesUnbundle* unbundle;

    /// <summary>
    /// The reference to the sync native method callback.
    /// </summary>
    ChakraBridge::CallSyncHandler^ callSyncHandler;

    /// <summary>
    /// The reference to the flush queue immediately callback.
    /// </summary>
    ChakraBridge::FlushQueueImmediateHandler^ flushQueueImmediateHandler;

private:
    JsErrorCode InitJson();
    JsErrorCode InitConsole();
	JsErrorCode InitNativeRequire();
    JsErrorCode InitNativeCallSyncHook();
    JsErrorCode InitFlushQueueImmediate();

    unsigned currentSourceContext;
    JsRuntimeHandle runtime;
    JsContextRef context;
    JsValueRef jsonParseObject;
    JsValueRef jsonStringifyObject;
};
