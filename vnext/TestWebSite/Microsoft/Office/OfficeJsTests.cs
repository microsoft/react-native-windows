using System.Collections.Concurrent;
using System.Text;
using System.Text.RegularExpressions;

namespace Microsoft.Office.Test
{
  public sealed class OfficeJsTests
  {
    private static readonly ConcurrentDictionary<string, int> s_issue4972RequestCounts = new();

    public static async Task Issue4144(HttpContext context)
    {
      var response = context.Response;
      response.ContentType = "text/plain";
      response.StatusCode = 200;

      var bytes = Encoding.UTF8.GetBytes("Check headers: [Access-Control-Allow-Origin]");
      await response.Body.WriteAsync(bytes);
    }

    public static async Task Issue5869(HttpContext context)
    {
      var response = context.Response;
      response.ContentType = "text/plain";
      response.StatusCode = 200;

      var request = context.Request;

      string? resBody;
      if (Regex.IsMatch(request.ContentType!, @"multipart/form-data(;\s+.*)?"))
      {
        resBody = $"Multipart form data with {request.Form.Count} entries";
      }
      else if (request.ContentType == "application/x-www-form-urlencoded")
      {
        resBody = $"URL-encoded form data with {request.Form.Count} entries";
      }
      else
      {
        resBody = $"Unknown Content-Type [{request.ContentType}]";
      }

      var bytes = Encoding.UTF8.GetBytes(resBody);
      await response.Body.WriteAsync(bytes);
    }

    public static async Task Issue4972(string id, HttpContext context)
    {
      var requestCount = s_issue4972RequestCounts.AddOrUpdate(id, 1, static (_, current) => current + 1);
      if (requestCount == 1)
      {
        context.Abort();
        return;
      }

      var response = context.Response;
      response.ContentType = "text/plain";
      response.StatusCode = 200;

      var bytes = Encoding.UTF8.GetBytes(requestCount.ToString());
      await response.Body.WriteAsync(bytes);
    }
  }
}
