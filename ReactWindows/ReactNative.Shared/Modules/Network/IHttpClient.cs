// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Web.Http;
#else
using System.Net.Http;
#endif

namespace ReactNative.Modules.Network
{
    /// <summary>
    /// An interface for HTTP clients.
    /// </summary>
    public interface IHttpClient : IDisposable
    {
        /// <summary>
        /// Send an asynchronous HTTP request.
        /// </summary>
        /// <param name="request">The HTTP request.</param>
        /// <param name="token">A cancellation token.</param>
        /// <returns>The HTTP response.</returns>
        Task<HttpResponseMessage> SendRequestAsync(HttpRequestMessage request, CancellationToken token);
    }
}
