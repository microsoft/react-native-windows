// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reactive.Disposables;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Temporary NullLoggingActivityBuilder.
    /// </summary>
    public class NullLoggingActivityBuilder
    {
        /// <summary>
        /// Dummy method to satisfy interface requirements.
        /// </summary>
        /// <returns>An empty disposable object.</returns>
        public IDisposable Start()
        {
            return Disposable.Empty;
        }

        /// <summary>
        /// Dummy method to satisfy interface requirements.
        /// </summary>
        /// <returns>An empty disposable object.</returns>
        public IDisposable Create()
        {
            return Disposable.Empty;
        }
    }
}