using System.Net.WebSockets;
using System.Text;

namespace Microsoft.React.Test
{
  public sealed class WebSocketTests
  {
    static List<WebSocket> wsConnections = new List<WebSocket>();

    public static async Task EchoSuffix(HttpContext context)
    {
      var announcement = @"This will send each incoming message back, with the string '_response' appended.";
      await Console.Out.WriteLineAsync(announcement);

      using var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

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

    public static async Task Pong(HttpContext context)
    {
      using var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

      while (true)
      {
        if (ws.State == WebSocketState.Open)
        {
          async Task<string> receiveMessage(WebSocket socket)
          {
            var inputBytes = new byte[4];
            WebSocketReceiveResult result;
            int total = 0;
            do
            {
              result = await socket.ReceiveAsync(new ArraySegment<byte>(inputBytes), CancellationToken.None);
              total += result.Count;
            } while (result != null && !result.EndOfMessage);

            return Encoding.UTF8.GetString(inputBytes, 0, total);
          };
          var inputMessage = await receiveMessage(ws);//TODO: Use byte(s) instead of string

          var outputMessage = "";
          var outputBytes = Encoding.UTF8.GetBytes(outputMessage);

          await ws.SendAsync(outputBytes, WebSocketMessageType.Binary, true, CancellationToken.None);
        }
        else if (ws.State == WebSocketState.Closed || ws.State == WebSocketState.Aborted)
        {
          break;
        }
      }
    }
  }
}
