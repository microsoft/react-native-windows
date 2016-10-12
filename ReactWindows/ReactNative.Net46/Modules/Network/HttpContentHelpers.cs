using System;
using System.IO;
using System.IO.Compression;
using System.Net.Http;
using System.Net.Http.Headers;

namespace ReactNative.Modules.Network
{
    static class HttpContentHelpers
    {
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope", Justification = "Caller responsible for disposing content.")]
        public static HttpContent CreateFromBody(HttpContentHeaderData headerData, string body)
        {
            if (headerData.ContentEncoding == "gzip")
            {
                var content = CreateGzip(body);
                content.Headers.ContentType = new MediaTypeHeaderValue(headerData.ContentType);
                content.Headers.ContentEncoding.Add(headerData.ContentEncoding);
                return content;
            }
            else
            {
                var content = CreateString(body);
                content.Headers.ContentType = new MediaTypeHeaderValue(headerData.ContentType);
                return content;
            }
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
        private static HttpContent CreateGzip(string body)
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
            return new StreamContent(stream);
        }

        private static HttpContent CreateString(string body)
        {
            return new StringContent(body);
        }
    }
}
