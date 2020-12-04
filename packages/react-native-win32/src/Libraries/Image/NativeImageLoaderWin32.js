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

import type {TurboModule} from '../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  +getConstants: () => {||};
  // [Win32 uses callback instead of promise
  +getSize: (
    uri: string,
    callback: (width: number, height: number, err?: string) => void,
  ) => void;
  // Win32]

  // [Win32 These aren't actually implemented, and will just blow up if called
  // currently.
  +getSizeWithHeaders: (
    uri: string,
    headers: Object,
  ) => Promise<{
    width: number,
    height: number,
    ...
  }>;
  +prefetchImage: (uri: string) => Promise<boolean>;
  +prefetchImageWithMetadata?: (
    uri: string,
    queryRootName: string,
    rootTag: number,
  ) => Promise<boolean>;
  +queryCache: (uris: Array<string>) => Promise<Object>;
  // Win32]
}

export default (TurboModuleRegistry.getEnforcing<Spec>('ImageLoader'): Spec);
