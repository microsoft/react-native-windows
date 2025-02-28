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
        if (ws.State == WebSocketState.Closed ||
          ws.State == WebSocketState.CloseSent ||
          ws.State == WebSocketState.CloseReceived ||
          ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        var inputMessage = await WebSocketUtils.ReceiveStringAsync(ws);
        await Console.Out.WriteLineAsync($"Received message: {inputMessage}");

        if (inputMessage == "exit")
          await Console.Out.WriteLineAsync("WebSocket integration test server exit");

        var outputMessage = $"{inputMessage}_response";
        var outputBytes = Encoding.UTF8.GetBytes(outputMessage);
        await ws.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
      }
    }

    public static async Task WebSocketBinaryTest(HttpContext context)
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

        var outgoingBytes = new byte[] { 4, 5, 6, 7 };

        await ws.SendAsync(outgoingBytes, WebSocketMessageType.Binary, true, CancellationToken.None);
      }
    }

    static Dictionary<string, WebSocket> multipleSendSocketsIn = new Dictionary<string, WebSocket>();
    static Dictionary<string, WebSocket> multipleSendSocketsOut = new Dictionary<string, WebSocket>();
    static Dictionary<string, Queue<byte[]>> multipleSendMessagesOut = new Dictionary<string, Queue<byte[]>>();

    public static async Task WebSocketMultipleSendTest_ClientSend(HttpContext context)
    {
      string? id = context.Request.RouteValues["Id"]!.ToString();
      if (string.IsNullOrEmpty(id))
      {
        await Console.Out.WriteLineAsync($"Invalid ID: {id}");
        return;
      }

      WebSocket ws;
      if (multipleSendSocketsIn.TryGetValue(id, out ws!))
        return; // WebSocket already registered for ID

      ws = await context.WebSockets.AcceptWebSocketAsync();
      await Console.Out.WriteLineAsync($"Accepted sender [{id}]");
      if (!multipleSendSocketsIn.TryAdd(id, ws!))
        return; //ERROR!

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
        await Console.Out.WriteLineAsync($"Received message: [{inputMessage}]");

        Queue<byte[]> outputQueue;
        if (! multipleSendMessagesOut.TryGetValue(id, out outputQueue!))
        {
          outputQueue = new Queue<byte[]>();
          multipleSendMessagesOut.Add(id, outputQueue);
        }
        outputQueue.Enqueue(Encoding.UTF8.GetBytes(inputMessage));
      }
    }

    public static async Task WebSocketMultipleSendTest_ClientReceive(HttpContext context)
    {
      string? id = context.Request.RouteValues["Id"]!.ToString();
      if (string.IsNullOrEmpty(id))
      {
        await Console.Out.WriteLineAsync($"Invalid ID: {id}");
        return;
      }

      WebSocket ws;
      if (multipleSendSocketsOut.TryGetValue(id, out ws!))
        return; // WebSocket already registered for ID

      ws = await context.WebSockets.AcceptWebSocketAsync();
      await Console.Out.WriteLineAsync($"Accepted receiver [{id}]");
      if (!multipleSendSocketsOut.TryAdd(id, ws!))
        return; //ERROR!

      while (true)
      {
        if (ws.State == WebSocketState.Closed ||
          ws.State == WebSocketState.CloseSent ||
          ws.State == WebSocketState.CloseReceived ||
          ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        Queue<byte[]> outputQueue;
        if (! multipleSendMessagesOut.TryGetValue(id, out outputQueue!))
        {
          multipleSendMessagesOut.Add(id, new Queue<byte[]>());
          continue;
        }

        byte[] outputBytes;
        if (! outputQueue.TryDequeue(out outputBytes!))
          continue;

        await ws.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
        await Console.Out.WriteLineAsync($"Sent [{outputBytes.Length}] bytes");
      }
    }
  }
}
