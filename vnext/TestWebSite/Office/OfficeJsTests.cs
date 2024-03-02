

namespace Microsoft.Office.Test
{
  public sealed class OfficeJsTests
  {
    public static void Issue4144(HttpContext context)
    {
      var request = context.Request;
      var response = context.Response;

      if (request.Method == "OPTIONS")
      {
        /*
         serverArgs.Preflight.set(http::field::access_control_request_headers, "ArbitraryHeader");
         serverArgs.Preflight.set(http::field::access_control_allow_headers,   "ArbitraryHeader");
         serverArgs.Preflight.set(http::field::access_control_allow_origin,    s_crossOriginUrl);
         */
        response.StatusCode = 200;

        response.Headers.Referer = request.Headers.Origin;// + "/";
      }
      else
      {
        response.StatusCode = 200;
      }
    }
  }
}
