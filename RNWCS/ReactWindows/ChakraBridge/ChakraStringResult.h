// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

using namespace Platform;

namespace ChakraBridge {

/// <summary>
/// A compound result containing the string result and a <see cref="JsErrorCode" /> for the error.
/// </summary>
public value struct ChakraStringResult
{
    /// <summary>The <see cref="JsErrorCode" /> for the operation, JsNoError if no error has occurred.</summary>
    int ErrorCode;
    /// <summary>The string result for the operation.</summary>
    String^ Result;
};

};
