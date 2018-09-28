// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.DevSupport;
using System;

namespace ReactNative.Modules.Core
{
    /// <summary>
    /// A class for JavaScript exceptions where the an itemized (per frame) stack is available.
    /// </summary>
    public class JavaScriptAppException : JavaScriptException
    {
        /// <summary>
        /// Instantiates the <see cref="JavaScriptException"/>.
        /// </summary>
        /// <param name="message">The exception message.</param>
        /// <param name="stackTrace">The JavaScript stack trace.</param>
        public JavaScriptAppException(string message, IStackFrame[] stackTrace)
            : base(message, stackTrace.PrettyPrint())
        {
            JavaScriptStackTraceItemized = stackTrace;
        }

        /// <summary>
        /// The exception stack trace.
        /// </summary>
        public IStackFrame[] JavaScriptStackTraceItemized
        {
            get;
        }
    }
}
