// @ts-check
/**
 * @typedef {import("metro").AssetData} AssetData;
 **/

/**
 * @param {AssetData} asset
 * @returns {Promise<AssetData>}
 */
async function metroShortPathAssetDataPlugin(asset) {
  asset['__useShortPath'] = true;
  return Promise.resolve(asset);
  }

module.exports = metroShortPathAssetDataPlugin;