/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<f30998a4fe53bae3f0c0c072677e946b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\EventEmitter\RCTNativeAppEventEmitter.js
 */

import RCTDeviceEventEmitter from "./RCTDeviceEventEmitter";
declare const RCTNativeAppEventEmitter: typeof RCTDeviceEventEmitter;
/**
 * Receive events from native-code
 * Deprecated - subclass NativeEventEmitter to create granular event modules instead of
 * adding all event listeners directly to RCTNativeAppEventEmitter.
 * @see https://github.com/facebook/react-native/blob/0.34-stable\Libraries\EventEmitter\RCTNativeAppEventEmitter.js
 * @see https://reactnative.dev/docs/native-modules-ios#sending-events-to-javascript
 */
declare const $$RCTNativeAppEventEmitter: typeof RCTNativeAppEventEmitter;
declare type $$RCTNativeAppEventEmitter = typeof $$RCTNativeAppEventEmitter;
export default $$RCTNativeAppEventEmitter;
