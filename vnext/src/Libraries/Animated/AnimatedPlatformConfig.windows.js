/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

'use strict';

export type PlatformConfig = {useComposition: boolean};

let defaultPlatformConfig: ?PlatformConfig = {useComposition: true};

export function unstable_getDefaultPlatformConfig(): ?PlatformConfig {
  return defaultPlatformConfig;
}

export function unstable_setDefaultPlatformConfig(
  platformConfig: ?PlatformConfig,
) {
  defaultPlatformConfig = platformConfig;
}
