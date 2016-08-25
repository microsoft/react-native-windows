#pragma once

#include <jsrt.h>
#include <set>

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> with the given depth and a list of values already seen.
/// </summary>
JsErrorCode StringifyJsValue(JsValueRef value, USHORT depth, std::set<JsValueRef> seen);

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> as a boolean value.
/// </summary>
JsErrorCode StringifyJsBoolean(JsValueRef value);

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> as a string value.
/// </summary>
JsErrorCode StringifyJsString(JsValueRef value);

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> as a function value.
/// </summary>
JsErrorCode StringifyJsFunction(JsValueRef value);

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> object with the given depth and a list of values already seen.
/// </summary>
JsErrorCode StringifyJsObject(JsValueRef value, USHORT depth, std::set<JsValueRef> seen);

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> array with the given depth and a list of values already seen.
/// </summary>
JsErrorCode StringifyJsArray(JsValueRef value, USHORT depth, std::set<JsValueRef> seen);

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> as a typed array value.
/// </summary>
JsErrorCode StringifyJsTypedArray(JsValueRef value);

/// <summary>
/// Stringifies a given <see cref="JsValueRef"/> by calling .toString() on the value.
/// </summary>
JsErrorCode StringifyToString(JsValueRef value);
