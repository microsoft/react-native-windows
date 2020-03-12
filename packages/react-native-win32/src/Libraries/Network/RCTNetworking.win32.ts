/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
'use strict';

// Do not require the native RCTNetworking module directly! Use this wrapper module instead.
// It will add the necessary requestId, so that you don't have to generate it yourself.
const NativeEventEmitter = require('../EventEmitter/NativeEventEmitter');

const convertRequestBody = require('./convertRequestBody');

import NativeNetworkingAndroid from './NativeNetworkingAndroid';
import type {RequestBody} from './convertRequestBody';

type Header = [string, string];

function convertHeadersMapToArray(headers: Object): Array<Header> {
  const headerArray = [];
  /* tslint:disable:forin */
  for (const name in headers) {
    /* tslint:disable:forin */
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
class RCTNetworking extends NativeEventEmitter {
  public constructor() {
    super(RCTNetworkingNative);
  }

  public sendRequest(
    method: string,
    trackingName: string,
    url: string,
    headers: Object,
    data: string | FormData | { uri: string },
    responseType: 'text' | 'base64',
    incrementalUpdates: boolean,
    timeout: number,
    /* tslint:disable:no-any */
    callback: (requestId: number) => any
    /* tslint:enable:no-any */
  ) {
    const body = typeof data === 'string' ? { string: data } : data instanceof FormData ? { formData: getParts(data) } : data;
    const requestId = generateRequestId();
    RCTNetworkingNative.sendRequest(
      method,
      url,
      requestId,
      convertHeadersMapToArray(headers),
      { ...body, trackingName },
      responseType,
      incrementalUpdates,
      timeout
    );
    callback(requestId);
  }

  public abortRequest(requestId: number) {
    RCTNetworkingNative.abortRequest(requestId);
  }

  /* tslint:disable:no-any */
  public clearCookies(callback: (result: boolean) => any) {
    /* tslint:enable:no-any */
    RCTNetworkingNative.clearCookies(callback);
  }
}

module.exports = (new RCTNetworking(): RCTNetworking);
