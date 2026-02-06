/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import resolveAssetSource from './resolveAssetSource.js'; // Get base impl
import Platform from '../Utilities/Platform';
import ensureShortPath from './assetPaths';

type IPackagerAsset = {
  __packager_asset: boolean,
  fileSystemLocation: string,
  httpServerLocation: string,
  width?: ?number,
  height?: ?number,
  scales: Array<number>,
  hash: string,
  name: string,
  type: string,
};

type IAssetResolver = {
  serverUrl?: ?string,
  // where the bundle is being run from
  bundleUrl?: ?string,
  // the asset to resolve
  asset: IPackagerAsset,
};

// Wrapper on top of Facebook's Asset resolver that keeps scaling info in the returned asset so that the native side can do the resolution
class AssetResolverLateScaleResolution {
  _resolver: IAssetResolver;

  constructor(resolver: IAssetResolver) {
    this._resolver = resolver;
  }

  defaultAsset() {
    if (this._isLoadedFromServer()) {
      return this._assetServerURL();
    }
    return this._scaledAssetURLInBundle();
  }

  _isLoadedFromServer(): boolean {
    return !!this._resolver.serverUrl;
  }

  /**
   * Resolves to where the bundle is running from, with a asset filename
   * E.g. 'file:///sdcard/bundle/assets/AwesomeModule/icon.png'
   */
  _scaledAssetURLInBundle() {
    const path = this._resolver.bundleUrl || 'file://';
    return this._fromSource(path + this._getAssetPath(true));
  }

  /**
   * Returns an absolute URL which can be used to fetch the asset
   * from the devserver
   */
  _assetServerURL() {
    return this._fromSource(
      this._resolver.serverUrl +
        this._getAssetPath(false) +
        '?platform=' +
        Platform.OS +
        '&hash=' +
        this._resolver.asset.hash,
    );
  }

  /**
   * Returns a path like 'assets/AwesomeModule/icon.png'
   */
  _getAssetPath(local: boolean): string {
    const assetDir = this._getBasePath(local);
    return (
      assetDir +
      '/' +
      this._resolver.asset.name +
      '.' +
      this._resolver.asset.type
    );
  }

  _getBasePath(local: boolean) {
    let basePath = this._resolver.asset.httpServerLocation;
    if (basePath[0] === '/') {
      basePath = basePath.substr(1);
    }

    if (local) {
      const safePath = basePath.replace(/\.\.\//g, '_');
      // If this asset was created with the newer saveAssetPlugin, then we should shorten the path
      // This conditional is added to allow back compat of older bundles which might have been created without the saveAssetPlugin
      if (this._resolver.asset.__useShortPath) {
        return ensureShortPath(safePath);
      }
      return safePath;
    }

    return basePath;
  }

  _fromSource(source: string) {
    return {
      __packager_asset: true,
      width: this._resolver.asset.width,
      height: this._resolver.asset.height,
      uri: source,
      // Include scales info in returned object
      // This may make it easier to do scale lookups on native side if we don't have that information from any kind of manifest
      scales: this._resolver.asset.scales,
    };
  }
}

// We should leave the resource scale out of the URI, and do that lookup on the native side.
// That way we can handle dynamic dpi changes and multimon scenarios better
function setCustomSourceTransformer(resolver) {
  const lsrResolver = new AssetResolverLateScaleResolution(resolver);
  return lsrResolver.defaultAsset();
}

resolveAssetSource.setCustomSourceTransformer = setCustomSourceTransformer;
export default resolveAssetSource;
