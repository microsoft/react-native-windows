/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import RCTDeviceEventEmitter from '../EventEmitter/RCTDeviceEventEmitter';
const RCTNetworkingNative =
  require('../BatchedBridge/NativeModules').Networking; // [Windows]
import {type NativeResponseType} from './XMLHttpRequest';
import convertRequestBody, {type RequestBody} from './convertRequestBody';
import {type EventSubscription} from '../vendor/emitter/EventEmitter';

type RCTNetworkingEventDefinitions = $ReadOnly<{
  didSendNetworkData: [
    [
      number, // requestId
      number, // progress
      number, // total
    ],
  ],
  didReceiveNetworkResponse: [
    [
      number, // requestId
      number, // status
      ?{[string]: string}, // responseHeaders
      ?string, // responseURL
    ],
  ],
  didReceiveNetworkData: [
    [
      number, // requestId
      string, // response
    ],
  ],
  didReceiveNetworkIncrementalData: [
    [
      number, // requestId
      string, // responseText
      number, // progress
      number, // total
    ],
  ],
  didReceiveNetworkDataProgress: [
    [
      number, // requestId
      number, // loaded
      number, // total
    ],
  ],
  didCompleteNetworkResponse: [
    [
      number, // requestId
      string, // error
      boolean, // timeOutError
    ],
  ],
}>;

let _requestId = 1;
function generateRequestId(): number {
  return _requestId++;
}

const RCTNetworking = {
  addListener<K: $Keys<RCTNetworkingEventDefinitions>>(
    eventType: K,
    listener: (...$ElementType<RCTNetworkingEventDefinitions, K>) => mixed,
    context?: mixed,
  ): EventSubscription {
    return RCTDeviceEventEmitter.addListener(eventType, listener, context);
  },

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
    const requestId = generateRequestId();
    const body = convertRequestBody(data);
    RCTNetworkingNative.sendRequest(
      {
        method,
        url,
        requestId,
        data: {...body, trackingName},
        headers,
        responseType,
        incrementalUpdates,
        timeout,
        withCredentials,
      },
      callback,
    );
  },

  abortRequest(requestId: number) {
    RCTNetworkingNative.abortRequest(requestId);
  },

  clearCookies(callback: (result: boolean) => void) {
    RCTNetworkingNative.clearCookies(callback);
  },
};

module.exports = RCTNetworking;
