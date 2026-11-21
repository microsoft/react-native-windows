/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8ad1db0d46c85560c15d956e03b2b96b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\assets\AssetRegistry.js
 */

export type AssetDestPathResolver = "android" | "generic";
export type PackagerAsset = Readonly<{
  fileSystemLocation: string;
  httpServerLocation: string;
  width: number | undefined;
  height: number | undefined;
  scales: Array<number>;
  hash: string;
  name: string;
  type: string;
  resolver?: AssetDestPathResolver | undefined;
}>;
/**
 * Runtime registry that maps asset IDs generated in a Metro bundle to asset
 * metadata. It backs `<Image>`, `Image.resolveAssetSource()`, and any code
 * that resolves `require('./img.png')` on native.
 *
 * Most apps do not use this directly — assets are handled through `<Image>`.
 */
export declare const AssetRegistry: {
  /**
   * Register an asset. Returns the asset ID.
   */
  registerAsset(asset: PackagerAsset): number;
  /**
   * Retrieve a registered asset by ID.
   */
  getAssetByID(assetId: number): PackagerAsset;
};
export declare type AssetRegistry = typeof AssetRegistry;
