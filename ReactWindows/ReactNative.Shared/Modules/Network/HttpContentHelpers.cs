// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.IO.Compression;
#if WINDOWS_UWP
using Windows.Web.Http;
using Windows.Web.Http.Headers;
using HttpContentType = Windows.Web.Http.IHttpContent;
#else
using System.Net.Http;
using System.Net.Http.Headers;
using HttpContentType = System.Net.Http.HttpContent;
using HttpStringContent = System.Net.Http.StringContent;
using HttpMediaTypeHeaderValue = System.Net.Http.Headers.MediaTypeHeaderValue;
#endif

namespace ReactNative.Modules.Network
{
    static class HttpContentHelpers
    {
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope", Justification = "Caller responsible for disposing content.")]
        public static HttpContentType CreateFromBody(HttpContentHeaderData headerData, string body)
        {
            if (headerData.ContentEncoding == "gzip")
            {
                var content = CreateGzip(body);
                content.Headers.ContentType = HttpMediaTypeHeaderValue.Parse(headerData.ContentType);
#if WINDOWS_UWP
                content.Headers.ContentEncoding.ParseAdd(headerData.ContentEncoding);
#else
                content.Headers.ContentEncoding.Add(headerData.ContentEncoding);
#endif
                return content;
            }
            else
            {
                var content = CreateString(body);
                content.Headers.ContentType = HttpMediaTypeHeaderValue.Parse(headerData.ContentType);
                return content;
            }
        }

        public static HttpContentType CreateFromBase64(HttpContentHeaderData headerData, string base64)
        {
            var content = CreateBase64(base64);
            content.Headers.ContentType = HttpMediaTypeHeaderValue.Parse(headerData.ContentType);
            return content;
        }

        public static HttpContentHeaderData ExtractHeaders(string[][] headers)
        {
            var result = new HttpContentHeaderData();

            foreach (var header in headers)
            {
                var key = header[0];
                switch (key.ToLowerInvariant())
                {
                    case "content-type":
                        result.ContentType = header[1];
                        break;
                    case "content-encoding":
                        result.ContentEncoding = header[1];
                        break;
                    default:
                        break;
                }
            }

            return result;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope")]
        private static HttpContentType CreateGzip(string body)
        {
            var stream = new MemoryStream();

            var gzipStream = new GZipStream(stream, CompressionMode.Compress, true);

            try
            {
                using (var streamWriter = new StreamWriter(gzipStream))
                {
                    gzipStream = null;
                    streamWriter.Write(body);
                }
            }
            finally
            {
                if (gzipStream != null)
                {
                    gzipStream.Dispose();
                }
            }

            stream.Position = 0;

#if WINDOWS_UWP
            return new HttpStreamContent(stream.AsInputStream());
#else
            return new StreamContent(stream);
#endif
        }

        private static HttpContentType CreateString(string body)
        {
            return new HttpStringContent(body);
        }

        private static HttpContentType CreateBase64(string base64)
        {
            var stream = new MemoryStream(Convert.FromBase64String(base64));
#if WINDOWS_UWP
            return new HttpStreamContent(stream.AsInputStream());
#else
            return new StreamContent(stream);
#endif
        }
    }
}
