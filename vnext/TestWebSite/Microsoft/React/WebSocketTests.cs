using System.Net.WebSockets;
using System.Text;

namespace Microsoft.React.Test
{
  public sealed class WebSocketTests
  {
    static List<WebSocket> wsConnections = new List<WebSocket>();

    public static async Task Echo(HttpContext context)
    {
      using var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

      while (true)
      {
        if (ws.State == WebSocketState.Closed ||
          ws.State == WebSocketState.CloseSent ||
          ws.State == WebSocketState.CloseReceived ||
          ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        var inputMessage = await WebSocketUtils.ReceiveStringAsync(ws);
        await Console.Out.WriteLineAsync($"Received message: {inputMessage}");

        var outputMessage = inputMessage;
        var outputBytes = Encoding.UTF8.GetBytes(outputMessage);

        await ws.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
      }
    }

    public static async Task EchoBinary(HttpContext context)
    {
      var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

      while (true)
      {
        if (ws.State == WebSocketState.Closed ||
          ws.State == WebSocketState.CloseSent ||
          ws.State == WebSocketState.CloseReceived ||
          ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        var incomingMessage = await WebSocketUtils.ReceiveStringAsync(ws);
        await Console.Out.WriteLineAsync($"Message received: [{incomingMessage}]");

        var outgoingBytes = Encoding.UTF8.GetBytes(incomingMessage);

        await ws.SendAsync(outgoingBytes, WebSocketMessageType.Binary, true, CancellationToken.None);
      }
    }

    public static async Task EchoSuffix(HttpContext context)
    {
      var announcement = @"This will send each incoming message back, with the string '_response' appended.";
      await Console.Out.WriteLineAsync(announcement);

      using var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

      while (true)
      {
        if (ws.State == WebSocketState.Closed ||
          ws.State == WebSocketState.CloseSent ||
          ws.State == WebSocketState.CloseReceived ||
          ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        var inputMessage = await WebSocketUtils.ReceiveStringAsync(ws);
        await Console.Out.WriteLineAsync($"Received message: {inputMessage}");

        var outputMessage = $"{inputMessage}_response";
        var outputBytes = Encoding.UTF8.GetBytes(outputMessage);

        await ws.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
      }
      ws.Dispose();
    }

    public static async Task Pong(HttpContext context)
    {
      using var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

      while (true)
      {
        if (ws.State == WebSocketState.Closed ||
          ws.State == WebSocketState.CloseSent ||
          ws.State == WebSocketState.CloseReceived ||
          ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        var inputMessage = await WebSocketUtils.ReceiveStringAsync(ws);

        var outputMessage = "";
        var outputBytes = Encoding.UTF8.GetBytes(outputMessage);

        await ws.SendAsync(outputBytes, WebSocketMessageType.Binary, true, CancellationToken.None);
      }
    }
  }
}
