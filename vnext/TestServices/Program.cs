using System.Net.WebSockets;
using System.Text;

var builder = WebApplication.CreateBuilder(args);
var webHost = builder.WebHost;
webHost.UseUrls(
  "http://localhost:5555",
  "https://localhost:5543"
  );

var app = builder.Build();
app.UseWebSockets();

app.Map("/h0", () => "HTTP Response #0");

// ws://localhost:5555
// See react-native/IntegrationTests/websocket_integration_test_server.js
// Note, the referred code has been removed in React Native 0.73
var wsConnections = new List<WebSocket>();
app.Map("/", async context =>
{
  if (!context.WebSockets.IsWebSocketRequest)
  {
    context.Response.StatusCode = (int)System.Net.HttpStatusCode.BadRequest;
    return;
  }

  //TODO: Rename. Not a message.
  var announcement = @"
WebSocket integration test server

This will send each incoming message back, with the string '_response' appended.
An incoming message of 'exit' will shut down the server.
";
  await Console.Out.WriteLineAsync(announcement);

  using var ws = await context.WebSockets.AcceptWebSocketAsync();
  wsConnections.Add(ws);

  if (ws.State == WebSocketState.Open)
  {
    var connectMessage = "hello";
    var bytes = Encoding.UTF8.GetBytes(connectMessage);
    var segment = new ArraySegment<byte>(bytes);

    await ws.SendAsync(segment, WebSocketMessageType.Text, true, CancellationToken.None);
  }

  while (true)
  {
    if (ws.State == WebSocketState.Open)
    {
      async Task<string> receiveMessage(WebSocket socket)
      {
        // Read incoming message
        var inputBytes = new byte[1024];
        WebSocketReceiveResult result;
        int total = 0;
        do
        {
          result = await socket.ReceiveAsync(new ArraySegment<byte>(inputBytes), CancellationToken.None);
          total += result.Count;
        } while (result != null && !result.EndOfMessage);

        return Encoding.UTF8.GetString(inputBytes, 0, total);
      };
      var inputMessage = await receiveMessage(ws);
      await Console.Out.WriteLineAsync($"Received message: {inputMessage}");

      if (inputMessage == "exit")
      {
        await Console.Out.WriteLineAsync("WebSocket integration test server exit");
      }

      var outputMessage = $"{inputMessage}_response";
      var outputBytes = Encoding.UTF8.GetBytes(outputMessage);
      var outputSegment = new ArraySegment<byte>(outputBytes);

      await ws.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None); 
    }
    else if (ws.State == WebSocketState.Closed || ws.State == WebSocketState.Aborted)
    {
      break;
    }
  }
});

await app.RunAsync();
