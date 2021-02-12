# node-rnw-rpc

node-rnw-rpc adds support for remote procedure calls from a node client to react-native-windows server on the same machine.

## Example

_node app_
```ts
import {waitForConnection} from 'node-rnw-rpc'

const rpcConnection = await waitForConnection({port: 8305});
const result = await rpcConnection.invoke("add", [2, 2])
```

_react-native-windows app_
```c++
#include "winrt/NodeRpc.Server.h"

// NodeRPC::Handler allows registering methods
winrt::NodeRPC::Handler handler();

// Binding to a simple method
handler.BindOperation("add", [](const JSonValue& params) noexcept {
  auto addends = params.GetArray();

  auto sum = 0;
  for (const auto& addend : addends) {
    sum += addend.GetNumber();
  }
  
  return JSonValue::CreateNumberValue(sum);
});

// Methods may be bound to IAsyncAction or IAsyncOperation
handler.BindAsyncAction("performAsyncOperation", [](const JSonValue& params) noexcept -> IAsyncAction {
  co_await performLongOperation();
});

// Start server
winrt::NodeRPC::Server rpcServer(handler);
co_await rpcServer.ProcessAllClientRequests(8305, 50ms);
```

## Installing
node-rnw-rpc supports [auto-linking](https://microsoft.github.io/react-native-windows/docs/native-modules-autolinking) to allow installation into react-native-windows applications.


## Architecture
**Reverse TCP server:** Traditional server/client roles are reversed, where the node client creates a TCP server to connect to. This helps to bypass restrictions on inbound loopback traffic to UWP apps. This low-effort solution allows bypassing the UWP AppContainer.

### Alternative approaches
- **Named pipe created inside AppContainer:** Node provides first-class support for named-pipes through `net.socket`. It is possible to create a named pipe from a UWP process which a Win32 process can then connect to, allowing traditional server/client roles. Doing this requires locating the resource local to the AppContainer, using an API like `GetAppContainerNamedObjectPath`. This likely requires FFI (with node-gyp) or native extenssions.
- **Proxy to full-trust process** The RNW application could create a full-trust process which proxies from TCP to named-pipe internal to the AppContainer. This bypasses inbound loopback restrictions, but adds complexity of deploying a separate process, requires two IPC channels.

### Protocol
node-rnw-rpc uses a TCP channel, sending JSON messages across the wire prefixed with length. Messages themselves are conformant to the [JSON-RPC](https://en.wikipedia.org/wiki/JSON-RPC) version 2.0 protocol.
