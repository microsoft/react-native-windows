// Copyright (c) Microsoft Corporation.
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
  public class ReactPromise<T> : IReactPromise<T>
  {
    private IJSValueWriter m_writer;
    private MethodResultCallback m_resolve;
    private MethodResultCallback m_reject;
    private object m_mutex = new object();

    public ReactPromise(IJSValueWriter writer, MethodResultCallback resolve, MethodResultCallback reject)
    {
      m_writer = writer;
      m_resolve = resolve;
      m_reject = reject;
    }

    //  Successfully resolve the ReactPromise with an optional value.
    public void Resolve(T value)
    {
      // Copy to local variables to run resolve outside of lock
      MethodResultCallback resolve = null;
      IJSValueWriter writer = null;

      lock (m_mutex)
      {
        if (m_resolve != null)
        {
          resolve = m_resolve;
          writer = m_writer;
        }

        Clear();
      }

      if (resolve != null)
      {
        writer.WriteArgs(value);
        resolve(writer);
      }
    }

    // Reject the ReactPromise and report an error.
    public void Reject(ReactError error)
    {
      // Copy to local variables to run reject outside of lock
      MethodResultCallback reject = null;
      IJSValueWriter writer = null;

      lock (m_mutex)
      {
        if (m_reject != null)
        {
          reject = m_reject;
          writer = m_writer;
        }

        Clear();
      }

      if (reject != null)
      {
        ReactPromiseWriter.WriteError(writer,
          code: error.Code,
          message: error.Message,
          userInfo: error.UserInfo,
          stackTrace: error.Exception?.StackTrace);
        reject(writer);
      }
    }

    public void Reject(string errorMessage)
    {
      Reject(new ReactError(errorMessage));
    }

    private void Clear()
    {
      m_resolve = null;
      m_reject = null;
      m_writer = null;
    }
  }

  public static class ReactPromiseWriter
  {
    public static void WriteError(
      IJSValueWriter writer,
      string code,
      string message,
      IReadOnlyDictionary<string, JSValue> userInfo,
      string stackTrace)
    {
      writer.WriteArrayBegin();
      {
        writer.WriteObjectBegin();
        {
          writer.WritePropertyName(ReactError.CodeKey);
          writer.WriteString(code ?? ReactError.DefaultCode);

          writer.WritePropertyName(ReactError.MessageKey);
          writer.WriteString(message ?? ReactError.DefaultMessage);

          // For consistency with iOS ensure userInfo key exists, even if we null it.
          // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
          writer.WritePropertyName(ReactError.UserInfoKey);
          if (userInfo == null)
          {
            writer.WriteNull();
          }
          else
          {
            writer.WriteValue(userInfo);
          }

          // Attach a nativeStackWindows string if an exception was passed.
          // This matches iOS behavior - iOS adds a `nativeStackIOS` property
          // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
          writer.WritePropertyName(ReactError.NativeStackKey);
          writer.WriteString(stackTrace ?? string.Empty);
        }
        writer.WriteObjectEnd();

      }
      writer.WriteArrayEnd();
    }
  }

  public class ReactPromise : ReactPromise<JSValue>
  {
    public ReactPromise(IJSValueWriter writer, MethodResultCallback resolve, MethodResultCallback reject)
      : base(writer, resolve, reject) { }

    public void Resolve()
    {
      Resolve(JSValue.Null);
    }
  }
}
