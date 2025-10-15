using System.Text;

namespace Microsoft.Office.Test
{
  public sealed class OfficeJsTests
  {
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

      var resBody = "";
      if (request.ContentType == "multipart/form-data")
      {
        resBody = $"Binary form data with {request.Form.Count} entries";
      }
      else if (request.ContentType == "application/x-www-form-urlencoded")
      {
        resBody = $"Text form data with {request.Form.Count} entries";
      }
      else
      {
        resBody = $"Unknown Content-Type [{request.ContentType}]";
      }

      var bytes = Encoding.UTF8.GetBytes(resBody);
      await response.Body.WriteAsync(bytes);
    }
  }
}
