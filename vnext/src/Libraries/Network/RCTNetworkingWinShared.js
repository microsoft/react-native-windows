/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict-local
 */

'use strict';

import NativeEventEmitter from '../EventEmitter/NativeEventEmitter';
import type {NativeResponseType} from './XMLHttpRequest';
import convertRequestBody from './convertRequestBody';

const RCTNetworkingNative = require('../BatchedBridge/NativeModules')
  .Networking; // [Windows]

import type {RequestBody} from './convertRequestBody';

// FIXME: use typed events
class RCTNetworking extends NativeEventEmitter<$FlowFixMe> {
  constructor() {
    const disableCallsIntoModule =
      typeof global.__disableRCTNetworkingExtraneousModuleCalls === 'function'
        ? global.__disableRCTNetworkingExtraneousModuleCalls()
        : false;

    super(RCTNetworkingNative, {
      __SECRET_DISABLE_CALLS_INTO_MODULE_DO_NOT_USE_OR_YOU_WILL_BE_FIRED: disableCallsIntoModule,
    });
  }

  sendRequest(
    method: string,
    trackingName: string,
    url: string,
    headers: {...},
    data: RequestBody,
    responseType: NativeResponseType,
    incrementalUpdates: boolean,
    timeout: number,
    callback: (requestId: number) => void,
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

  clearCookies(callback: (result: boolean) => void) {
    RCTNetworkingNative.clearCookies(callback);
  }
}

module.exports = (new RCTNetworking(): RCTNetworking);
