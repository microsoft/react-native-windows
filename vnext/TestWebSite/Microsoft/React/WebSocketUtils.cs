using System.Net.WebSockets;
using System.Text;

namespace Microsoft.React.Test
{
  public sealed class WebSocketUtils
  {
    public static async Task<string> ReceiveStringAsync(WebSocket socket)
    {
      // Read incoming message
      WebSocketReceiveResult result;
      var bufffer = new byte[1024];
      var payload = new byte[1024];
      int total = 0;
      int lastTotal;
      try
      {
        do
        {
          result = await socket.ReceiveAsync(new ArraySegment<byte>(bufffer), CancellationToken.None);
          lastTotal = total;
          total += result.Count;
          if (total > payload.Length)
            Array.Resize(ref payload, total);

          Array.Copy(bufffer, 0, payload, lastTotal, result.Count);
        } while (result != null && !result.EndOfMessage);
      }
      catch (WebSocketException e)
      {
        //TODO: Investigate RNTesterIntegrationTests.
        if (e.Message != "The remote party closed the WebSocket connection without completing the close handshake.")
          throw;

        await Console.Out.WriteLineAsync($"[WARNING]: {e.Message}");
      }

      return Encoding.UTF8.GetString(payload, 0, total);
    }
  }
}
