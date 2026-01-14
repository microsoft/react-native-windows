// @ts-check
import path from 'path';
import ensureShortPath from './Libraries/Image/assetPaths');

/**
 * @typedef {import("metro").AssetData} AssetData;
 **/

/**
 * @param {AssetData} asset
 * @param {number} scale 
 * @returns {string}
 */
function getAssetDestPath(asset, scale) {
  const suffix = scale === 1 ? '' : `@${scale}x`;
  const fileName = `${asset.name + suffix}.${asset.type}`;
  return path.join(
    // Assets can have relative paths outside of the project root.
    // Replace `../` with `_` to make sure they don't end up outside of
    // the expected assets directory.
    ensureShortPath(asset.httpServerLocation.substr(1).replace(/\.\.\//g, '_')),
    fileName,
  );
}

/**
 * @param {ReadonlyArray<AssetData>} assets 
 * @param {string} _platform 
 * @param {string | undefined} _assetsDest 
 * @param {string | undefined} _assetCatalogDest 
 * @param {(asset: AssetData, allowedScales: number[] | undefined, getAssetDestPath: (asset: AssetData, scale: number) => string) => void} addAssetToCopy 
 */
function saveAssetsWin32(
  assets,
  _platform,
  _assetsDest,
  _assetCatalogDest,
  addAssetToCopy,
) {
  assets.forEach((asset) =>
    addAssetToCopy(asset, undefined, getAssetDestPath),
  );
}

module.exports = saveAssetsWin32;