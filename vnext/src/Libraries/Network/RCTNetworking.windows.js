/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

const NativeEventEmitter = require('NativeEventEmitter');
const RCTNetworkingNative = require('NativeModules').Networking;
const convertRequestBody = require('convertRequestBody');
const invariant = require('invariant');

import type {RequestBody} from 'convertRequestBody';

import type {NativeResponseType} from './XMLHttpRequest';

class RCTNetworking extends NativeEventEmitter {
  isAvailable: boolean = true;

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

if (__DEV__ && !RCTNetworkingNative) {
  // This module depends on the native `RCTAppTheme` module. If you don't include it,
  // `AppTheme.isAvailable` will return `false`, and any method calls will throw.
  class MissingNativeRCTNetworkingShim {
    isAvailable = false;

    throwMissingNativeModule() {
      invariant(
        false,
        'Cannot use Networking module when native RCTNetworking is not included in the build.\n' +
          'Either include it, or check Networking.isAvailable before calling any methods.',
      );
    }

    addEventListener() {
      this.throwMissingNativeModule();
    }

    removeEventListener() {
      this.throwMissingNativeModule();
    }

    // EventEmitter
    addListener(_eventType, _listener) {
      this.throwMissingNativeModule();
    }

    removeAllListeners() {
      this.throwMissingNativeModule();
    }

    removeListener(_eventType, _listener) {
      this.throwMissingNativeModule();
    }

    removeSubscription() {
      this.throwMissingNativeModule();
    }

    sendRequest(...args: Array<any>) {
      this.throwMissingNativeModule();
    }

    abortRequest(...args: Array<any>) {
      this.throwMissingNativeModule();
    }

    clearCookies(...args: Array<any>) {
      this.throwMissingNativeModule();
    }
  }

  // This module depends on the native `RCTNetworkingNative` module. If you don't include it,
  // `RCTNetworking.isAvailable` will return `false`, and any method calls will throw.
  // We reassign the class variable to keep the autodoc generator happy.
  RCTNetworking = new MissingNativeRCTNetworkingShim();
} else {
  RCTNetworking = new RCTNetworking();
}

module.exports = RCTNetworking;
