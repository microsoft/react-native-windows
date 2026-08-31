const { getDefaultConfig, mergeConfig } = require('@react-native/metro-config');
const fs = require('fs');
const path = require('node:path');
// escape-string-regexp v5 is ESM-only: require() gives a namespace, not a fn.
const escape = (s) => s.replace(/[|\\{}()[\]^$+*?.]/g, '\\$&');
const pack = require('../package.json');

const root = path.resolve(__dirname, '..');
const modules = Object.keys({ ...pack.peerDependencies });

// create-react-native-library 0.63 moved the library's source entry from a
// top-level "source" field into exports["."] under a "<slug>-source" condition;
// support both shapes.
const sourceCondition = `${pack.name.replace(/^@/, '').replace(/\//g, '-')}-source`;
const librarySource = pack.source ?? pack.exports?.['.']?.[sourceCondition];

// On Windows, require.resolve through yarn workspace junctions can return paths
// with a different drive letter case than process.cwd(). Metro's internal file
// system lookup is case-sensitive, so we normalize to match cwd.
function normalizePathDrive(p) {
  if (process.platform === 'win32' && p.length >= 2 && p[1] === ':') {
    return process.cwd()[0] + p.slice(1);
  }
  return p;
}

const rnwPath = normalizePathDrive(fs.realpathSync(
  path.resolve(require.resolve('react-native-windows/package.json'), '..'),
));

//{{#devMode}} [devMode
const rnwRootNodeModules = path.resolve(rnwPath, '..', 'node_modules');
const rnwPackages = path.resolve(rnwPath, '..', 'packages');
// devMode]{{/devMode}}

/**
 * Metro configuration
 * https://facebook.github.io/metro/docs/configuration
 *
 * @type {import('metro-config').MetroConfig}
 */
const config = {
  watchFolders: [root,
    //{{#devMode}} [devMode
    rnwPath, rnwRootNodeModules, rnwPackages
    // devMode]{{/devMode}}
  ],

  // We need to make sure that only one version is loaded for peerDependencies
  // So we block them at the root, and alias them to the versions in example's node_modules
  resolver: {
    // The example imports the library by its package name, but the library is neither
    // installed in the example's node_modules (no workspace junction) nor built, so metro
    // can't resolve it. Redirect that bare import to the library's source at the lib root.
    resolveRequest: (context, moduleName, platform) => {
      if (moduleName === pack.name) {
        return { type: 'sourceFile', filePath: path.resolve(root, librarySource) };
      }
      return context.resolveRequest(context, moduleName, platform);
    },
    blocklist: 
      modules.map(
        (m) =>
          new RegExp(`^${escape(path.join(root, 'node_modules', m))}\\/.*$`)
      ).concat([
        // This stops "npx @react-native-community/cli run-windows" from causing the metro server to crash if its already running
        new RegExp(
          `${path.resolve(__dirname, 'windows').replace(/[/\\]/g, '/')}.*`,
        ),
        // This prevents "npx @react-native-community/cli run-windows" from hitting: EBUSY: resource busy or locked, open msbuild.ProjectImports.zip or other files produced by msbuild
        new RegExp(`${rnwPath}/build/.*`),
        new RegExp(`${rnwPath}/target/.*`),
        /.*\.ProjectImports\.zip/,
      ]),

    extraNodeModules: modules.reduce((acc, name) => {
      acc[name] = path.join(__dirname, 'node_modules', name);
      return acc;
    },
    {
      //{{#devMode}} [devMode
      'react-native-windows': rnwPath,
      // devMode]{{/devMode}}
    }
    ),
  },

  transformer: {
    getTransformOptions: async () => ({
      transform: {
        experimentalImportSupport: false,
        inlineRequires: true,
      },
    }),
  },
};

module.exports = mergeConfig(getDefaultConfig(__dirname), config);
