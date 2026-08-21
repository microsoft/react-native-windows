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

    public static async Task EchoCookie(HttpContext context)
    {
      using var ws = await context.WebSockets.AcceptWebSocketAsync();
      wsConnections.Add(ws);

      var cookie = context.Request.Headers.Cookie.ToString();

      while (true)
      {
        if (ws.State == WebSocketState.Closed ||
          ws.State == WebSocketState.CloseSent ||
          ws.State == WebSocketState.CloseReceived ||
          ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        try
        {
          await WebSocketUtils.ReceiveStringAsync(ws);
        }
        catch (WebSocketException)
        {
          break;
        }

        if (ws.State != WebSocketState.Open)
          break;

        var responseBytes = Encoding.UTF8.GetBytes(cookie);

        try
        {
          await ws.SendAsync(responseBytes, WebSocketMessageType.Text, true, CancellationToken.None);
        }
        catch (WebSocketException)
        {
          break;
        }
      }
    }

    public static async Task EchoBinaryGrow(HttpContext context)
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

        var inputBuffer = new byte[1024];
        var payload = new List<byte>();

        while (true)
        {
          var segment = new ArraySegment<byte>(inputBuffer);
          var receiveResult = await ws.ReceiveAsync(segment, CancellationToken.None);

          if (receiveResult.MessageType == WebSocketMessageType.Close)
          {
            await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
            return;
          }

          for (var i = 0; i < receiveResult.Count; i++)
          {
            payload.Add(inputBuffer[i]);
          }

          if (receiveResult.EndOfMessage)
            break;
        }

        payload.Add((byte)(payload.Count + 1));
        var outputBytes = payload.ToArray();

        await ws.SendAsync(outputBytes, WebSocketMessageType.Binary, true, CancellationToken.None);
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
