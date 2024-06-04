/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';

export type ResolvedAssetSource = {|
  +__packager_asset: boolean,
  +width: ?number,
  +height: ?number,
  +uri: string,
  +scale: number,
|};

import type {PackagerAsset} from '@react-native/assets-registry/registry';

const PixelRatio = require('../Utilities/PixelRatio').default;
const Platform = require('../Utilities/Platform');
const {pickScale} = require('./AssetUtils');
const {
  getAndroidResourceFolderName,
  getAndroidResourceIdentifier,
} = require('@react-native/assets-registry/path-support');
const invariant = require('invariant');
// $FlowFixMe[untyped-import]
const ensureShortPath = require('./assetPaths.js'); // [Windows]

// [Windows - instead of using basePath from @react-native/assets-registry/path-support]
function getBasePath(asset: PackagerAsset, local: boolean) {
  let basePath = asset.httpServerLocation;
  if (basePath[0] === '/') {
    basePath = basePath.substr(1);
  }

  if (local) {
    const safePath = basePath.replace(/\.\.\//g, '_');
    // If this asset was created with saveAssetPlugin, then we should shorten the path
    // This conditional allow compat of bundles which might have been created without the saveAssetPlugin
    // $FlowFixMe: __useShortPath not part of public type
    if (asset.__useShortPath) {
      return ensureShortPath(safePath);
    }
    return safePath;
  }

  return basePath;
}

/**
 * Returns a path like 'assets/AwesomeModule/icon@2x.png'
 */
function getScaledAssetPath(asset: PackagerAsset, local: boolean): string {
  const scale = pickScale(asset.scales, PixelRatio.get());
  const scaleSuffix = scale === 1 ? '' : '@' + scale + 'x';
  const assetDir = getBasePath(asset, local);
  return assetDir + '/' + asset.name + scaleSuffix + '.' + asset.type;
}

/**
 * Returns a path like 'drawable-mdpi/icon.png'
 */
function getAssetPathInDrawableFolder(asset: PackagerAsset): string {
  const scale = pickScale(asset.scales, PixelRatio.get());
  const drawableFolder = getAndroidResourceFolderName(asset, scale);
  const fileName = getAndroidResourceIdentifier(asset);
  return drawableFolder + '/' + fileName + '.' + asset.type;
}

class AssetSourceResolver {
  serverUrl: ?string;
  // where the jsbundle is being run from
  jsbundleUrl: ?string;
  // the asset to resolve
  asset: PackagerAsset;

  constructor(serverUrl: ?string, jsbundleUrl: ?string, asset: PackagerAsset) {
    this.serverUrl = serverUrl;
    this.jsbundleUrl = jsbundleUrl;
    this.asset = asset;
  }

  isLoadedFromServer(): boolean {
    return !!this.serverUrl;
  }

  isLoadedFromFileSystem(): boolean {
    return this.jsbundleUrl != null && this.jsbundleUrl?.startsWith('file://');
  }

  defaultAsset(): ResolvedAssetSource {
    if (this.isLoadedFromServer()) {
      return this.assetServerURL();
    }

    if (Platform.OS === 'android') {
      return this.isLoadedFromFileSystem()
        ? this.drawableFolderInBundle()
        : this.resourceIdentifierWithoutScale();
    } else {
      return this.scaledAssetURLNearBundle();
    }
  }

  /**
   * Returns an absolute URL which can be used to fetch the asset
   * from the devserver
   */
  assetServerURL(): ResolvedAssetSource {
    invariant(this.serverUrl != null, 'need server to load from');
    return this.fromSource(
      this.serverUrl +
        getScaledAssetPath(this.asset, false) +
        '?platform=' +
        Platform.OS +
        '&hash=' +
        this.asset.hash,
    );
  }

  /**
   * Resolves to just the scaled asset filename
   * E.g. 'assets/AwesomeModule/icon@2x.png'
   */
  scaledAssetPath(local: boolean): ResolvedAssetSource {
    return this.fromSource(getScaledAssetPath(this.asset, local));
  }

  /**
   * Resolves to where the bundle is running from, with a scaled asset filename
   * E.g. 'file:///sdcard/bundle/assets/AwesomeModule/icon@2x.png'
   */
  scaledAssetURLNearBundle(): ResolvedAssetSource {
    const path = this.jsbundleUrl ?? 'file://';
    return this.fromSource(
      // Assets can have relative paths outside of the project root.
      // When bundling them we replace `../` with `_` to make sure they
      // don't end up outside of the expected assets directory.
      path + getScaledAssetPath(this.asset, true).replace(/\.\.\//g, '_'),
    );
  }

  /**
   * The default location of assets bundled with the app, located by
   * resource identifier
   * The Android resource system picks the correct scale.
   * E.g. 'assets_awesomemodule_icon'
   */
  resourceIdentifierWithoutScale(): ResolvedAssetSource {
    invariant(
      Platform.OS === 'android',
      'resource identifiers work on Android',
    );
    return this.fromSource(getAndroidResourceIdentifier(this.asset));
  }

  /**
   * If the jsbundle is running from a sideload location, this resolves assets
   * relative to its location
   * E.g. 'file:///sdcard/AwesomeModule/drawable-mdpi/icon.png'
   */
  drawableFolderInBundle(): ResolvedAssetSource {
    const path = this.jsbundleUrl ?? 'file://';
    return this.fromSource(path + getAssetPathInDrawableFolder(this.asset));
  }

  fromSource(source: string): ResolvedAssetSource {
    return {
      __packager_asset: true,
      width: this.asset.width,
      height: this.asset.height,
      uri: source,
      scale: pickScale(this.asset.scales, PixelRatio.get()),
    };
  }

  static pickScale: (scales: Array<number>, deviceScale?: number) => number =
    pickScale;
}

module.exports = AssetSourceResolver;
