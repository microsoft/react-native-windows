/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

import type {RootTag} from '../../../../Libraries/TurboModule/RCTExport';
import type {TurboModule} from '../../../../Libraries/TurboModule/RCTExport';

import * as TurboModuleRegistry from '../../../../Libraries/TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  readonly getConstants: () => {};
  // [Win32 uses callback instead of promise
  readonly getSize: (
    uri: string,
    callback: (width: number, height: number, err?: string) => void,
  ) => void;
  // Win32]

  // [Win32 These aren't actually implemented, and will just blow up if called
  // currently.
  readonly getSizeWithHeaders: (
    uri: string,
    headers: Object,
  ) => Promise<{
    width: number,
    height: number,
    ...
  }>;
  readonly prefetchImage: (uri: string) => Promise<boolean>;
  readonly prefetchImageWithMetadata?: (
    uri: string,
    queryRootName: string,
    rootTag: RootTag,
  ) => Promise<boolean>;
  readonly queryCache: (uris: Array<string>) => Promise<Object>;
  // Win32]
}

export default TurboModuleRegistry.getEnforcing<Spec>('ImageLoader') as Spec;
