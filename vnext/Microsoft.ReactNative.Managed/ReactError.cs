// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed
{
  public struct ReactError
  {
    public string Code { get; set; }
    public string Message { get; set; }
    public Exception Exception { get; set; }
    public IReadOnlyDictionary<string, JSValue> UserInfo { get; set; }

    public ReactError(string code, string message, Exception exception = null, IReadOnlyDictionary<string, JSValue> userInfo = null)
    {
      Code = code ?? DefaultCode;
      Message = message ?? DefaultMessage;
      Exception = exception;
      UserInfo = userInfo;
    }

    public ReactError(string message, Exception exception = null, IReadOnlyDictionary<string, JSValue> userInfo = null)
      : this(null, message, exception, userInfo) { }

    public static readonly string DefaultCode = "EUNSPECIFIED";
    public static readonly string DefaultMessage = "Error not specified.";

    // Keys for m_reject's Error object
    public static readonly string CodeKey = "code";
    public static readonly string MessageKey = "message";
    public static readonly string UserInfoKey = "userInfo";
    public static readonly string NativeStackKey = "nativeStackWindows";
  }
}
