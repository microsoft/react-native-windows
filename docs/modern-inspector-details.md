# Modern Inspector Support in React Native for Windows

## Overview
The modern inspector is the Chrome DevTools–based debugging experience that ships with the latest
versions of React Native. This experience now works end-to-end for React Native for Windows (RNW)
applications, enabling parity with Android and iOS. The integration provides a unified way to inspect
JavaScript execution, evaluate console expressions, profile CPU and memory usage, and visualize the
component hierarchy for both the Paper and Composition UI stacks.

## Modern Inspector Building Blocks
- **Host Target** – One per `ReactHost`; surfaces metadata, reload requests, pause overlays, and
  implements the CDP-facing delegate (`ReactInspectorHostTargetDelegate`).
- **Instance Target** – Created for each React instance; registers runtime targets, tracks instance
  lifecycle, and unregisters cleanly on reload.
- **Runtime Target & Agent** – Runtime targets map to JavaScript VMs; agents are per-session objects
  that translate Chrome DevTools Protocol (CDP) messages into engine calls. RNW mirrors upstream
  lifetimes so reloads tear everything down deterministically.
- **Frontend Channel** – Delivers JSON CDP payloads between the RNW host and DevTools.
- **Inspector Thread** – A single `ReactInspectorThread` ensures CDP work is serialized away from
  the UI and JS queues. (iOS and Andrtoid use UI thread.)
- **Debugger Notifications** – `DebuggerNotifications.h` broadcasts pause/resume events so view
  hosts can react (e.g., showing overlays or resuming when the debugger continues).

## Windows Integration Points
- **ReactHost & ReactOptions** – `ReactHost` creates the `HostTarget`, exposes it through
  `ReactOptions::InspectorHostTarget`, and implements reload/pause hooks. This is the jump-off point
  for all inspector traffic. The inspector supported only if the `UseDirectDebugger` is true.
- **ReactInstanceWin / OInstance** – Register and unregister instance/runtime targets around runtime
  creation, keeping the inspector aligned with bridgeless and legacy bridge lifecycles.
- **DevSupportManager & Packager** – `DevSupportManager` now spins up
  `ReactInspectorPackagerConnectionDelegate`, allowing Metro to broker modern inspector connections
  and reuse the existing websocket infrastructure.
- **Dev Console Shortcut** – Metro’s `J` shortcut launches the inspector for Windows apps, matching
  upstream behavior.

## UI Overlays
- **Composition** – `DebuggerUIIsland` renders pause overlays, focus chrome, and selection adorners
  whenever the runtime is paused.
- **Paper** – `ReactRootView` updates provide the same pause/selection affordances.

## Hermes Runtime Integration
- `HermesRuntimeTargetDelegate` and `HermesRuntimeAgentDelegate` wrap the hermes-windows C debug API
  so we can re-use upstream modern inspector code.
- `RuntimeHolder`/`HermesRuntimeHolder` surface a `createRuntimeTargetDelegate` hook that instantiates
  delegates only when the inspector is enabled, and safely tears them down during reloads.

## Packager & Console Integration
- `ReactInspectorPackagerConnectionDelegate` maps the Metro websocket APIs to the modern inspector
  handshake.
- Console output, CPU sampling, and memory profiling are forwarded through the Hermes inspector
  plumbing automatically once a session is active.

## Using the Modern Inspector with RNW
1. Start your Metro bundler (`npx react-native start` or `yarn start`).
2. Launch your RNW app (Paper or Composition).
3. In the Metro console, press `J` to open the modern inspector URL in a browser.
4. Chrome DevTools will connect to the Hermes runtime. Pause execution, explore the component tree,
  and capture profiles as needed.
5. When execution is paused, the corresponding overlay is rendered in the app window; resume to clear
  the overlay.

## Known Limitations & Follow-Up Work
- **Network profiling** – The `NetworkIOAgent` is not wired up yet for Windows. The integration point
  is the `ReactInspectorHostTargetDelegate` in
  `vnext/Microsoft.ReactNative/ReactHost/ReactHost.cpp`; override `loadNetworkResource` there to
  forward requests through a Windows HTTP helper (similar to `GetJavaScriptFromServerAsync`) and
  stream results back via the provided `NetworkRequestListener`. Until this happens, the Network tab
  in DevTools stays empty.
- **Legacy Chakra runtime** – Modern inspector support currently targets Hermes. Chakra-based apps
  continue to rely on legacy debugging flows.
