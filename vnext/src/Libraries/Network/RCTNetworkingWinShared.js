/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

const NativeEventEmitter = require('../EventEmitter/NativeEventEmitter');
const RCTNetworkingNative = require('../BatchedBridge/NativeModules')
  .Networking;
const convertRequestBody = require('./convertRequestBody');

import type {RequestBody} from './convertRequestBody';

import type {NativeResponseType} from './XMLHttpRequest';

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
