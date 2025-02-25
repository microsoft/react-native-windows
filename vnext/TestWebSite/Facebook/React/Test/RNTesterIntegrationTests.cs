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

    static Dictionary<string, WebSocket> multipleSendSocketsIn = new Dictionary<string, WebSocket>();
    static Dictionary<string, WebSocket> multipleSendSocketsOut = new Dictionary<string, WebSocket>();
    static Dictionary<string, Queue<byte[]>> multipleSendMessagesOut = new Dictionary<string, Queue<byte[]>>();

    public static async Task WebSocketMultipleSendTest_ClientSend(HttpContext context)
    {
      string? id = context.Request.RouteValues["Id"]!.ToString();
    }

    public static async Task WebSocketMultipleSendTest_ClientReceive(HttpContext context)
    {
      string? id = context.Request.RouteValues["Id"]!.ToString();
      if (string.IsNullOrEmpty(id))
      {
        await Console.Out.WriteLineAsync($"Invalid ID: {id}");
        return;
      }

      var ws = await context.WebSockets.AcceptWebSocketAsync();
      await Console.Out.WriteLineAsync($"Accepted receiver [{id}]");

      while (true)
      {
        if (ws.State == WebSocketState.Closed || ws.State == WebSocketState.Aborted)
          break;

        if (ws.State != WebSocketState.Open)
          continue;

        Queue<byte[]> outputQueue;
        if (! multipleSendMessagesOut.TryGetValue(id, out outputQueue!))
        {
          continue;
        }
        byte[] outputBytes;
        if (! outputQueue.TryDequeue(out outputBytes!))
        {
          continue;
        }

        await ws.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
        await Console.Out.WriteLineAsync($"Sent message: {outputBytes}");
      }
    }


    public static async Task WebSocketMultipleSendTest(HttpContext context)
    {
      // string? clientAction = context.Request.RouteValues["Action"]!.ToString();
      string? id = context.Request.RouteValues["Id"]!.ToString();
      // if (!("send" == clientAction || "receive" == clientAction) || string.IsNullOrEmpty(id))
      // {
      //   await Console.Out.WriteLineAsync($"Invalid path: [{clientAction}/{id}]");
      //   return;
      // }

      // if ("send" == clientAction)
      //   await WebSocketMultipleSendRead(context, id);
      // else if ("receive" == clientAction)
      //   await WebsSocketMultipleSendWrite(context, id);

      var senderClient = await context.WebSockets.AcceptWebSocketAsync();
      await Console.Out.WriteLineAsync("Accepted sender");
      var receiverClient = await context.WebSockets.AcceptWebSocketAsync();
      await Console.Out.WriteLineAsync("Accepted receiver");

      while (true)
      {
        if (senderClient.State == WebSocketState.Open && receiverClient.State == WebSocketState.Open)
        {
          var inputMessage = await WebSocketUtils.ReceiveStringAsync(senderClient);
          await Console.Out.WriteLineAsync($"Received message: {inputMessage}");

          var outputBytes = Encoding.UTF8.GetBytes(inputMessage);
          await receiverClient.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
          await Console.Out.WriteLineAsync($"Sent message: {outputBytes}");
        }
        else if (senderClient.State == WebSocketState.Closed || senderClient.State == WebSocketState.Aborted ||
            receiverClient.State == WebSocketState.Closed || receiverClient.State == WebSocketState.Aborted)
        {
          break;
        }
      }

#if false
      //TODO: Simplify using pointers.
      WebSocket wsIn = null, wsOut = null;

      if ("send" == clientAction)
      {
        if (multipleSendSocketsIn.ContainsKey(id))
        {
          // wsIn = multipleSendSocketsIn[id];
          return;
        }
        else
        {
          wsIn = await context.WebSockets.AcceptWebSocketAsync();
          multipleSendSocketsIn.Add(id, wsIn);
          wsConnections.Add(wsIn);
          await Console.Out.WriteLineAsync($"Connection accepted at {DateTime.UtcNow}");
        }

        if (! multipleSendSocketsOut.ContainsKey(id))
        {
          return;
        }

        wsOut = multipleSendSocketsOut[id];
      }
      else if ("receive" == clientAction)
      {
        if (multipleSendSocketsOut.ContainsKey(id))
        {
          //wsOut = multipleSendSocketsOut[id];
          return;
        }
        else
        {
          wsOut = await context.WebSockets.AcceptWebSocketAsync();
          multipleSendSocketsOut.Add(id, wsOut);
          wsConnections.Add(wsOut);
          await Console.Out.WriteLineAsync($"Connection accepted at {DateTime.UtcNow}");
        }

        if (! multipleSendSocketsIn.ContainsKey(id))
        {
          return;
        }

          //TODO: Disposed when retrieved
          wsIn = multipleSendSocketsIn[id];
      }

      while (true)
      {
        if (wsIn.State == WebSocketState.Open)
        {
          var inputMessage = await WebSocketUtils.ReceiveStringAsync(wsIn);
          await Console.Out.WriteLineAsync($"Received message: {inputMessage}");

          var outputBytes = Encoding.UTF8.GetBytes(inputMessage);
          await wsOut.SendAsync(outputBytes, WebSocketMessageType.Text, true, CancellationToken.None);
          await Console.Out.WriteLineAsync($"Sent message: {outputBytes}");
        }
        else if (wsIn.State == WebSocketState.Closed || wsIn.State == WebSocketState.Aborted)
        {
          break;
        }
      }
#endif
    }
  }
}
