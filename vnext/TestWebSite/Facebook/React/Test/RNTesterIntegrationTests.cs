using System.Net.WebSockets;
using System.Text;

using WebSocketUtils = Microsoft.React.Test.WebSocketUtils;

namespace Facebook.React.Test
{
  public sealed class RNTesterIntegrationTests
  {
    static List<WebSocket> wsConnections = new List<WebSocket>();

    public static async Task WebSocketTest(HttpContext context)
    {
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
          var inputMessage = await WebSocketUtils.ReceiveStringAsync(ws);
          await Console.Out.WriteLineAsync($"Received message: {inputMessage}");

          if (inputMessage == "exit")
          {
            await Console.Out.WriteLineAsync("WebSocket integration test server exit");
          }

          var outputMessage = $"{inputMessage}_response";
          var outputBytes = Encoding.UTF8.GetBytes(outputMessage);

          await ws.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
        }
        else if (ws.State == WebSocketState.Closed || ws.State == WebSocketState.Aborted)
        {
          break;
        }
      }
    }

    public static async Task WebSocketBinaryTest(HttpContext context)
    {
      var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

      while (true)
      {
        if (ws.State == WebSocketState.Open)
        {
          var incomingMessage = await WebSocketUtils.ReceiveStringAsync(ws);
          await Console.Out.WriteLineAsync($"Message received: [{incomingMessage}]");

          var outgoingBytes = new byte[] { 4, 5, 6, 7 };

          await ws.SendAsync(outgoingBytes, WebSocketMessageType.Binary, true, CancellationToken.None);
        }
        else if(ws.State == WebSocketState.Closed || ws.State == WebSocketState.Aborted)
        {
          break;
        }
      }
    }
  }
}
