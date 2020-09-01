/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

import NativeEventEmitter from '../EventEmitter/NativeEventEmitter';
import type {NativeResponseType} from './XMLHttpRequest';
import convertRequestBody from './convertRequestBody';

const RCTNetworkingNative = require('../BatchedBridge/NativeModules')
  .Networking; // [Windows]

import type {RequestBody} from './convertRequestBody';

class RCTNetworking extends NativeEventEmitter {
  constructor() {
    super(RCTNetworkingNative);
  }

  sendRequest(
    method: string,
    trackingName: string,
    url: string,
    headers: Object,
    data: RequestBody,
    responseType: NativeResponseType,
    incrementalUpdates: boolean,
    timeout: number,
    callback: (requestId: number) => any,
    withCredentials: boolean,
  ) {
    const body = convertRequestBody(data);
    RCTNetworkingNative.sendRequest(
      {
        method,
        url,
        data: {...body, trackingName},
        headers,
        responseType,
        incrementalUpdates,
        timeout,
        withCredentials,
      },
      callback,
    );
  }

  abortRequest(requestId: number) {
    RCTNetworkingNative.abortRequest(requestId);
  }

  clearCookies(callback: (result: boolean) => any) {
    RCTNetworkingNative.clearCookies(callback);
  }
}

module.exports = (new RCTNetworking(): RCTNetworking);
