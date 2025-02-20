using System;
using System.Net.WebSockets;
using System.Text;

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
          async Task<string> receiveMessage(WebSocket socket)
          {
            // Read incoming message
            var buffer = new byte[1024];
            var payload = new byte[1024];
            WebSocketReceiveResult result;
            int total = 0;
            int lastTotal;
            do
            {
              result = await socket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);

              lastTotal = total;
              total += result.Count;
              if (total > payload.Length)
                Array.Resize(ref payload, total);

              Array.Copy(buffer, 0, payload, lastTotal, result.Count);
            } while (result != null && !result.EndOfMessage);

            return Encoding.UTF8.GetString(payload, 0, total);
          };
          var inputMessage = await receiveMessage(ws);
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
          var incomingMessage = await receiveMessage(ws);
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
