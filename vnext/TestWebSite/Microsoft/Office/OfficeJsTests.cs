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
  }
}
