/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */
const fs = require('fs');
const path = require('path');
const blacklist = require('metro-config/src/defaults/blacklist');
const {resolve} = require('metro-resolver');

const rnwPath = fs.realpathSync(
  path.resolve(require.resolve('react-native-windows/package.json'), '..'),
);

const rnwPathNormalized = path.normalize(rnwPath);
const rnwSrcPathNormalized = path.normalize(rnwPath + '/src');

function isRelativeImport(filePath) {
  return /^[.][.]?(?:[/]|$)/.test(filePath);
}

// Example: vnextResolve('C:/Repos/react-native-windows/vnext/', './Libraries/Text/Text');
// Returns a full path to the resolved location which would be in vnext/src if the file exists there
// or just in vnext otherwise
function vnextResolve(originDir, moduleName) {
  const originDirSrc = originDir.replace(
    rnwPathNormalized,
    rnwSrcPathNormalized,
  );

  // redirect the resolution to src if an appropriate file exists there
  const extensions = [
    '.windows.tsx',
    '.windows.ts',
    '.windows.jsx',
    '.windows.js',
    '.tsx',
    '.ts',
    '.jsx',
    '.js',
  ];

  // For each potential extension we need to check for the file in either vnext/src, or in vnext
  for (let extension of extensions) {
    const potentialSrcModuleName =
      path.resolve(originDirSrc, moduleName) + extension;
    if (fs.existsSync(potentialSrcModuleName)) {
      return potentialSrcModuleName;
    }

    const potentialModuleName = path.resolve(originDir, moduleName) + extension;
    if (fs.existsSync(potentialModuleName)) {
      return potentialModuleName;
    }
  }
}

// This teaches metro how we merge vnext/src and files from core to vnext...
// Basically any resolves within vnext we look to see if the file exists in vnext/src,
// and use that one if it exists.  That way fast refresh will work on files within vnext/src
function vnextDevResolveRequest(
  context,
  _realModuleName /* string */,
  platform /* string */,
  moduleName /* string */,
) {
  let backupResolveRequest = context.resolveRequest;
  delete context.resolveRequest;

  try {
    let modifiedModuleName = moduleName;
    if (platform === 'windows') {
      if (moduleName === 'react-native') {
        // redirect react-native -> vnext/src/index.windows.js
        modifiedModuleName = path.resolve(rnwPath, 'src/index.windows.js');
      } else if (moduleName.startsWith('react-native/')) {
        // redirect react-native/foo -> vnext/src/foo or vnext/foo
        modifiedModuleName = vnextResolve(
          rnwPath,
          (moduleName = `./${moduleName.slice('react-native/'.length)}`),
        );
      } else if (
        context.originModulePath.startsWith(rnwPath) &&
        isRelativeImport(moduleName)
      ) {
        // We need to handle relative paths from within react-native-windows
        let originPathWithoutSrc = context.originModulePath;
        if (originPathWithoutSrc.startsWith(rnwSrcPathNormalized))
          originPathWithoutSrc = `${rnwPath}/${originPathWithoutSrc.slice(
            rnwSrcPathNormalized.length,
          )}`;

        const resolvedLocation = vnextResolve(
          path.dirname(originPathWithoutSrc),
          moduleName,
        );
        if (resolvedLocation) modifiedModuleName = resolvedLocation;
      }
    }
    let result = resolve(context, modifiedModuleName, platform);
    return result;
  } catch (e) {
    throw e;
  } finally {
    context.resolveRequest = backupResolveRequest;
  }
}

module.exports = {
  // WatchFolders is only needed due to the yarn workspace layout of node_modules, we need to watch the symlinked locations separately
  watchFolders: [
    // Include hoisted modules
    path.resolve(__dirname, '../..', 'node_modules'),
    // Include react-native-windows
    rnwPath,
  ],

  resolver: {
    extraNodeModules: {
      // Redirect react-native-windows to avoid symlink (metro doesn't like symlinks)
      'react-native-windows': rnwPath,
    },
    blacklistRE: blacklist([
      // Avoid error EBUSY: resource busy or locked, open 'D:\a\1\s\packages\playground\msbuild.ProjectImports.zip' in pipeline
      /.*\.ProjectImports\.zip/,

      // This stops "react-native run-windows" from causing the metro server to crash if its already running
      new RegExp(
        `${path.resolve(__dirname, 'windows').replace(/[/\\]/g, '/')}.*`,
      ),
    ]),
    resolveRequest: vnextDevResolveRequest,
  },

  // Metro doesn't currently handle assets from other packages within a monorepo.  This is the current workaround people use
  server: {
    enhanceMiddleware: middleware => {
      return (req, res, next) => {
        if (req.url.startsWith('/vnext')) {
          req.url = req.url.replace('/vnext', '/assets/../../vnext');
        }
        return middleware(req, res, next);
      };
    },
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
