var builder = WebApplication.CreateBuilder(args);
var webHost = builder.WebHost;
webHost.UseUrls(
  "http://localhost:5555",
  "https://localhost:5543"
  );

var app = builder.Build();
app.UseWebSockets();

// See https://github.com/dotnet/aspnetcore/blob/v7.0.15/src/Http/Routing/src/RequestDelegateRouteBuilderExtensions.cs
// app.Map("/", () => "Sample HTTP Response");
async Task DefaultRequestDelegate(HttpContext context)
{
  var response = context.Response;
  response.StatusCode = 200;
  response.ContentType = "text/plain";

  var bytes = System.Text.Encoding.UTF8.GetBytes("Sample HTTP Response");
  await response.Body.WriteAsync(bytes);
}

#region Request Mappings

app.Map("/", async context =>
{
  if (context.WebSockets.IsWebSocketRequest)
  {
    // ws://localhost:5555
    // See react-native/IntegrationTests/websocket_integration_test_server.js
    // Note, the referred code has been removed in React Native 0.73
    await Facebook.React.Test.RNTesterIntegrationTests.WebSocketTest(context);
  }
  else
  {
    await DefaultRequestDelegate(context);
  }
});

app.Map(
  "/Facebook/React/RNTesterIntegrationTests/WebSocketBinaryTest",
  Facebook.React.Test.RNTesterIntegrationTests.WebSocketBinaryTest
  );

#endregion Request Mappings

await app.RunAsync();
