/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<b187519b1745e53483788ab6add9a145>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Image\AssetSourceResolver.js
 */

import type { AssetDestPathResolver, PackagerAsset } from "../../src/private/assets/AssetRegistry";
export type ResolvedAssetSource = {
  readonly width: number | undefined;
  readonly height: number | undefined;
  readonly uri: string;
  readonly scale: number;
};
declare class AssetSourceResolver {
  serverUrl: null | undefined | string;
  jsbundleUrl: null | undefined | string;
  asset: PackagerAsset;
  constructor(serverUrl: null | undefined | string, jsbundleUrl: null | undefined | string, asset: PackagerAsset);
  isLoadedFromServer(): boolean;
  isLoadedFromFileSystem(): boolean;
  defaultAsset(): ResolvedAssetSource;
  getAssetUsingResolver(resolver: AssetDestPathResolver): ResolvedAssetSource;
  /**
   * Returns an absolute URL which can be used to fetch the asset
   * from the devserver
   */
  assetServerURL(): ResolvedAssetSource;
  /**
   * Resolves to just the scaled asset filename
   * E.g. 'assets/AwesomeModule/icon@2x.png'
   */
  scaledAssetPath(): ResolvedAssetSource;
  /**
   * Resolves to where the bundle is running from, with a scaled asset filename
   * E.g. 'file:///sdcard/bundle/assets/AwesomeModule/icon@2x.png'
   */
  scaledAssetURLNearBundle(): ResolvedAssetSource;
  /**
   * The default location of assets bundled with the app, located by
   * resource identifier
   * The Android resource system picks the correct scale.
   * E.g. 'assets_awesomemodule_icon'
   */
  resourceIdentifierWithoutScale(): ResolvedAssetSource;
  /**
   * If the jsbundle is running from a sideload location, this resolves assets
   * relative to its location
   * E.g. 'file:///sdcard/AwesomeModule/drawable-mdpi/icon.png'
   */
  drawableFolderInBundle(): ResolvedAssetSource;
  fromSource(source: string): ResolvedAssetSource;
  static pickScale: (scales: Array<number>, deviceScale?: number) => number;
}
export default AssetSourceResolver;
