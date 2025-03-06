var builder = WebApplication.CreateBuilder(args);

// CORS middleware
//TODO: Subordinate to specific set of tests
var originPolicyName = "AllowedOrigins";
builder.Services.AddCors(
  options =>
  {
    options.AddPolicy(
      name: originPolicyName,
      policy =>
      {
        policy
          .WithOrigins("http://orig.in")
          .WithMethods("GET")
          ;
      });
  });

var app = builder.Build();
app.UseWebSockets();
app.UseCors();
app.UseStaticFiles();

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
  "/rnw/rntester/websocketbinarytest",
  Facebook.React.Test.RNTesterIntegrationTests.WebSocketBinaryTest
  );

app.Map(
  "/rnw/rntester/websocketmultiplesendtest/send/{Id}",
  Facebook.React.Test.RNTesterIntegrationTests.WebSocketMultipleSendTest_ClientSend
);
app.Map(
  "/rnw/rntester/websocketmultiplesendtest/receive/{Id}",
  Facebook.React.Test.RNTesterIntegrationTests.WebSocketMultipleSendTest_ClientReceive
);

app.Map(
  "/rnw/websockets/echo",
  Microsoft.React.Test.WebSocketTests.Echo
  );

app.Map(
  "/rnw/websockets/echosuffix",
  Microsoft.React.Test.WebSocketTests.EchoSuffix
  );

app.Map(
  "/rnw/websockets/pong",
  Microsoft.React.Test.WebSocketTests.Pong
  );

app.MapGet(
  "/officedev/office-js/issues/4144",
   Microsoft.Office.Test.OfficeJsTests.Issue4144)
  .RequireCors(originPolicyName);

#endregion Request Mappings

await app.RunAsync();
