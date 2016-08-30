#pragma once

#include "pch.h"
#include <jsrt.h>

/// <summary>
/// This class wraps the main functionality dealing with the JSRT.
/// </summary>
class ChakraHost
{
public:
	/// <summary>
	/// Initializes a JSRT session with the context, globals, JSON, and console logging.
	///</summary>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode Init();

	/// <summary>
	/// Destorys the current JSRT session.
	///</summary>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode Destroy();

	/// <summary>
	/// Runs the given script with the source URI and returns the <see cref="JsValueRef" /> result.
	///</summary>
	/// <param name="szScript">The script to run.</param>
	/// <param name="szSourceUri">The source URI of the script.</param>
	/// <param name="result">[Out] The result from running the script.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode RunScript(const wchar_t* szScript, const wchar_t* szSourceUri, JsValueRef* result);

	/// <summary>
	/// Runs the script from the file location with the source URI and returns the <see cref="JsValueRef" /> result.
	///</summary>
	/// <param name="szPath">The location of the script to run.</param>
	/// <param name="szSourceUri">The source URI of the script.</param>
	/// <param name="result">[Out] The result from running the script.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode RunScriptFromFile(const wchar_t* szPath, const wchar_t* szSourceUri, JsValueRef* result);

	/// <summary>
	/// Calls JSON stringify on the given <see cref="JsValueRef" /> and returns the <see cref="JsValueRef" /> result.
	///</summary>
	/// <param name="argument">The argument to stringify.</param>
	/// <param name="result">[Out] The result from running JSON.stringify.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode JsonStringify(JsValueRef argument, JsValueRef* result);

	/// <summary>
	/// Calls JSON parse on the given <see cref="JsValueRef" /> and returns the <see cref="JsValueRef" /> result.
	///</summary>
	/// <param name="argument">The argument to parse.</param>
	/// <param name="result">[Out] The result from running JSON.parse.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode JsonParse(JsValueRef argument, JsValueRef* result);

	/// <summary>
	/// Gets a global variable and returns the <see cref="JsValueRef" /> result.
	///</summary>
	/// <param name="szPropertyName">The property name from global to get.</param>
	/// <param name="result">[Out] The global property value.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode GetGlobalVariable(const wchar_t* szPropertyName, JsValueRef* result);

	/// <summary>
	/// Sets a global variable with the <see cref="JsValueRef" />.
	///</summary>
	/// <param name="szPropertyName">The property name from global to get.</param>
	/// <param name="value">The global property value to set.</param>
	/// <returns>
	/// JsNoError is no error, else a JsErrorCode with the appropriate error.
	/// </returns>
	JsErrorCode SetGlobalVariable(const wchar_t* szPropertyName, JsValueRef value);

	/// <summary>
	/// The JSRT global object for the session.
	/// </summary>
	JsValueRef globalObject;
private:
	JsErrorCode InitJson();
	JsErrorCode InitConsole();

	unsigned currentSourceContext;
	JsRuntimeHandle runtime;
	JsContextRef context;
	JsValueRef jsonParseObject;
	JsValueRef jsonStringifyObject;
};
