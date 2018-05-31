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
    class DefaultHttpClient : IHttpClient
    {
        private readonly HttpClient _client;

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope", Justification = "HttpClient disposed by container.")]
        public DefaultHttpClient()
            : this(new HttpClient())
        {
        }

        public DefaultHttpClient(HttpClient client)
        {
            _client = client;
        }

        public async Task<HttpResponseMessage> SendRequestAsync(HttpRequestMessage request, CancellationToken token)
        {
#if WINDOWS_UWP
            var asyncInfo = _client.SendRequestAsync(request, HttpCompletionOption.ResponseHeadersRead);
            using (token.Register(asyncInfo.Cancel))
            {
                try
                {
                    return await asyncInfo.AsTask().ConfigureAwait(false);
                }
                catch (OperationCanceledException)
                {
                    token.ThrowIfCancellationRequested();
                    throw;
                }
            }
#else
            return await _client.SendAsync(request, token).ConfigureAwait(false);
#endif
        }

        public void Dispose()
        {
            _client.Dispose();
        }
    }
}
