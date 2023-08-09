const fs = require('fs');
const path = require('path');

// Check that we have built our JS files before running the bundler, otherwise we'll get a harder to diagnose "Unable to resolve module" error
if (
  !fs.existsSync(
    path.resolve(__dirname, 'Libraries/Components/View/ViewWin32.js'),
  )
) {
  throw new Error(
    '[31m\nThis package must be built before running the bundler.  Did you mean to run "[39m[33myarn build[39m[31m" first?[39m\n',
  );
}

const {makeMetroConfig} = require('@rnw-scripts/metro-dev-config');
module.exports = makeMetroConfig();
// Enable this when RN CLI gets support for saveAssetPlugins: https://github.com/react-native-community/cli/pull/2002
// module.exports.transformer.assetPlugins = [require.resolve('./metroShortPathAssetDataPlugin.js')];
