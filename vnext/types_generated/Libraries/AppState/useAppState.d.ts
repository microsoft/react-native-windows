/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8119619a722e718c8b18799896e6c2c6>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\AppState\useAppState.js
 */

import type { AppStateStatus } from "./AppState";
/**
 * `useAppState` is a React hook that returns the current app state.
 *
 * The value will be one of:
 * - `active` - The app is running in the foreground
 * - `background` - The app is running in the background
 * - `inactive` - (iOS only) Transitioning between foreground and background
 * - `unknown` - The initial state before the app state is determined
 *
 * The hook automatically subscribes to app state changes and re-renders the
 * component when the state changes.
 *
 * Usage:
 * ```
 * function MyComponent() {
 *   const appState = useAppState();
 *
 *   useEffect(() => {
 *     if (appState === 'active') {
 *       // App came to foreground - refresh data
 *     }
 *   }, [appState]);
 *
 *   return <Text>App is {appState}</Text>;
 * }
 * ```
 *
 * See https://reactnative.dev/docs/appstate
 */
declare function useAppState(): AppStateStatus;
export default useAppState;
