using System.Net.WebSockets;
using System.Text;

var builder = WebApplication.CreateBuilder(args);
var webHost = builder.WebHost;
webHost.UseUrls(
  "http://localhost:5555",
  "http://localhost:7777",
  "https://localhost:44355"
  );

var app = builder.Build();
app.UseWebSockets();

app.Map("/h0", () => "HTTP Response #0");

// ws://localhost:5555
// See react-native/IntegrationTests/websocket_integration_test_server.js
// Note, the referred code has been removed in React Native 0.73
//TODO: route ws://localhost:5555/ here
app.Map("/ws", async context =>
{
  //TODO: Print?
  //TODO: Rename. Not a message.
  var startupMessage = @"
WebSocket integration test server

This will send each incoming message back, with the string '_response' appended.
An incoming message of 'exit' will shut down the server.
";
  using var ws = await context.WebSockets.AcceptWebSocketAsync();

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
      // Read incoming message
      var inputBytes = new byte[1024];
      await ws.ReceiveAsync(new ArraySegment<byte>(inputBytes), CancellationToken.None);
      var inputMessage = Encoding.UTF8.GetString(inputBytes, 0, inputBytes.Length);
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
