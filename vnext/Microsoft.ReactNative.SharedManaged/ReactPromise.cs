// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// The code is adopted from Facebook's ReactNative implementation for Java.
// It is changed to be more C#-like, but we kept some original comments.
// Below is the original Facebook's copyright notice:
//
//
// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//

using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed
{
  // Implementation of IReactPromise that represents a JavaScript Promise
  // which can be passed to the native module as a method parameter.
  //
  // Methods with ReactMethodAttribute that use an IReactPromise as the last parameter
  // will be marked as "promise" and will return a promise when invoked from JavaScript.
  class ReactPromise<T> : IReactPromise<T>
  {
    private static readonly string ErrorDefaultCode = "EUNSPECIFIED";
    private static readonly string ErrorDefaultMessage = "Error not specified.";

    // Keys for m_reject's Error object
    private static readonly string ErrorMapKeyCode = "code";
    private static readonly string ErrorMapKeyMessage = "message";
    private static readonly string ErrorMapKeyUserInfo = "userInfo";
    private static readonly string ErrorMapKeyNativeStack = "nativeStackWindows";

    private IJSValueWriter m_writer;
    private MethodResultCallback m_resolve;
    private MethodResultCallback m_reject;

    public ReactPromise(IJSValueWriter writer, MethodResultCallback resolve, MethodResultCallback reject)
    {
      m_writer = writer;
      m_resolve = resolve;
      m_reject = reject;
    }

    //  Successfully resolve the ReactPromise with an optional value.
    public void Resolve(T value)
    {
      if (m_resolve != null)
      {
        m_writer.WriteArgs(value);
        m_resolve(m_writer);
      }

      Clear();
    }

    // Reject the ReactPromise and report an error.
    public void Reject(ReactError error)
    {
      if (m_reject != null)
      {
        var errorInfo = new Dictionary<string, JSValue>
        {
          [ErrorMapKeyCode] = new JSValue(error?.Code ?? ErrorDefaultCode),
          [ErrorMapKeyMessage] = new JSValue(error?.Message ?? error?.Exception?.Message ?? ErrorDefaultMessage),

          // For consistency with iOS ensure userInfo key exists, even if we null it.
          // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
          [ErrorMapKeyUserInfo] = (error?.UserInfo != null) ? new JSValue(error.UserInfo) : new JSValue(),

          // Attach a nativeStackWindows string if an exception was passed.
          // This matches iOS behavior - iOS adds a `nativeStackIOS` property
          // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
          [ErrorMapKeyNativeStack] = new JSValue(error?.Exception?.StackTrace ?? "")
        };

        m_writer.WriteArgs(errorInfo);
        m_reject(m_writer);
      }

      Clear();
    }

    private void Clear()
    {
      m_resolve = null;
      m_reject = null;
      m_writer = null;
    }
  }
}
