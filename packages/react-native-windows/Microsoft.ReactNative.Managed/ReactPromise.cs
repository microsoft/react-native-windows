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
                ReactPromiseWriter.WriteError(m_writer,
                  code: error?.Code,
                  message: error?.Message,
                  userInfo: error?.UserInfo,
                  stackTrace: error?.Exception?.StackTrace);
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
                    writer.WritePropertyName(ReactErrorConstants.Code);
                    writer.WriteString(code ?? ReactErrorConstants.DefaultCode);

                    writer.WritePropertyName(ReactErrorConstants.Message);
                    writer.WriteString(message ?? ReactErrorConstants.DefaultMessage);

                    // For consistency with iOS ensure userInfo key exists, even if we null it.
                    // iOS: /React/Base/RCTUtils.m -> RCTJSErrorFromCodeMessageAndNSError
                    writer.WritePropertyName(ReactErrorConstants.UserInfo);
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
                    writer.WritePropertyName(ReactErrorConstants.NativeStack);
                    writer.WriteString(stackTrace ?? string.Empty);
                }
                writer.WriteObjectEnd();

            }
            writer.WriteArrayEnd();
        }

    }
}
