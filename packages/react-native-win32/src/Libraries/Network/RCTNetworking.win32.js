/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

// Do not require the native RCTNetworking module directly! Use this wrapper module instead.
// It will add the necessary requestId, so that you don't have to generate it yourself.
import NativeEventEmitter from '../EventEmitter/NativeEventEmitter';
import convertRequestBody from './convertRequestBody';

// [Win32
import type {RequestBody} from './convertRequestBody';
const NativeModules = require('../BatchedBridge/NativeModules');
const RCTNetworkingNative = NativeModules.Networking;
// Win32]

type Header = [string, string];

// Convert FormData headers to arrays, which are easier to consume in
// native on Android.
function convertHeadersMapToArray(headers: Object): Array<Header> {
  const headerArray = [];
  for (const name in headers) {
    headerArray.push([name, headers[name]]);
  }
  return headerArray;
}

let _requestId = 1;
function generateRequestId(): number {
  return _requestId++;
}

/**
 * This class is a wrapper around the native RCTNetworking module. It adds a necessary unique
 * requestId to each network request that can be used to abort that request later on.
 */
// FIXME: use typed events
class RCTNetworking extends NativeEventEmitter<$FlowFixMe> {
  constructor() {
    super(RCTNetworkingNative); // [Win32] Use RCTNetworkingNative
  }

  sendRequest(
    method: string,
    trackingName: string,
    url: string,
    headers: Object,
    data: RequestBody,
    responseType: 'text' | 'base64',
    incrementalUpdates: boolean,
    timeout: number,
    callback: (requestId: number) => mixed,
    withCredentials: boolean,
  ) {
    const body = convertRequestBody(data);
    if (body && body.formData) {
      body.formData = body.formData.map(part => ({
        ...part,
        headers: convertHeadersMapToArray(part.headers),
      }));
    }
    const requestId = generateRequestId();
    RCTNetworkingNative.sendRequest(
      // [Win32] Use RCTNetworkingNative
      method,
      url,
      requestId,
      convertHeadersMapToArray(headers),
      {...body, trackingName},
      responseType,
      incrementalUpdates,
      timeout,
      withCredentials,
    );
    callback(requestId);
  }

  abortRequest(requestId: number) {
    RCTNetworkingNative.abortRequest(requestId); // [Win32] Use RCTNetworkingNative
  }

  clearCookies(callback: (result: boolean) => any) {
    RCTNetworkingNative.clearCookies(callback); // [Win32] Use RCTNetworkingNative
  }
}

module.exports = (new RCTNetworking(): RCTNetworking);
