namespace Microsoft.React.Test;

using System.Collections.Concurrent;

internal static class HttpTests
{
    private static readonly ConcurrentDictionary<string, int> s_cacheRequestCounts = new();

    public static IResult BasicGet()
    {
        return Results.Text("some response content", "text/plain");
    }

    public static IResult Headers(HttpContext context)
    {
        context.Response.Headers.Server = "Microsoft.React.Test.Website";
        context.Response.Headers.Append("ResponseHeaderName1", "ResponseHeaderValue1");
        context.Response.ContentLength = 0;

        return Results.Empty;
    }

    public static IResult UserAgent(HttpContext context)
    {
        context.Response.Headers.CacheControl = "no-store, no-cache, max-age=0";
        context.Response.Headers.Pragma = "no-cache";

        var userAgent = context.Request.Headers.UserAgent.ToString();

        return string.IsNullOrWhiteSpace(userAgent)
            ? Results.StatusCode(StatusCodes.Status403Forbidden)
            : Results.Ok();
    }

    public static IResult Options(HttpContext context)
    {
        context.Response.Headers.Append("PreflightName", "PreflightValue");

        return Results.Text(string.Empty, contentType: null, statusCode: StatusCodes.Status206PartialContent);
    }

    public static IResult OptionsGet()
    {
        return Results.Text("Response Body", "text/plain");
    }

    public static IResult RedirectGetStart(HttpContext context)
    {
        context.Response.StatusCode = StatusCodes.Status301MovedPermanently;
        context.Response.Headers.Location = "/rnw/http/redirect/get/final";

        return Results.Empty;
    }

    public static IResult RedirectGetFinal()
    {
        return Results.Text("Redirect Content", "text/plain");
    }

    public static IResult RedirectPatchStart(HttpContext context)
    {
        context.Response.StatusCode = StatusCodes.Status301MovedPermanently;
        context.Response.Headers.Location = "/rnw/http/redirect/patch/final";

        return Results.Empty;
    }

    public static IResult RedirectPatchFinal()
    {
        return Results.Text("Redirect Content", "text/plain");
    }

    public static async Task<IResult> Delay(int milliseconds)
    {
        await Task.Delay(milliseconds);
        return Results.Ok();
    }

    public static IResult Cache(string id, HttpContext context)
    {
        var requestCount = s_cacheRequestCounts.AddOrUpdate(id, 1, static (_, current) => current + 1);
        if (requestCount <= 3)
        {
            context.Response.Headers.CacheControl = "no-cache";
        }

        return Results.Text(Math.Min(requestCount, 4).ToString(), "text/plain");
    }
}
