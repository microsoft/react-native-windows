/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<c7be84ecb3968427c21e8ed1eb5cd080>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Network\RCTNetworkingEventDefinitions.flow.js
 */

export type RCTNetworkingEventDefinitions = Readonly<{
  didSendNetworkData: [[number, number, number]];
  didReceiveNetworkResponse: [[number, number, {
    [$$Key$$: string]: string;
  } | undefined, string | undefined]];
  didReceiveNetworkData: [[number, string]];
  didReceiveNetworkIncrementalData: [[number, string, number, number]];
  didReceiveNetworkDataProgress: [[number, number, number]];
  didCompleteNetworkResponse: [[number, string, boolean]];
}>;
