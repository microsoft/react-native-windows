const {makeMetroConfig} = require('@rnw-scripts/metro-dev-config');

/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */
const fs = require('fs');
const path = require('path');
const {resolve} = require('metro-resolver');

const rnwPath = fs.realpathSync(
  path.dirname(require.resolve('react-native-windows/package.json')),
);

const rnwTesterPath = fs.realpathSync(
  path.dirname(require.resolve('@react-native-windows/tester/package.json')),
);

const devPackages = {
  'react-native': path.normalize(rnwPath),
  'react-native-windows': path.normalize(rnwPath),
  '@react-native-windows/tester': path.normalize(rnwTesterPath),
};

function isRelativeImport(filePath) {
  return /^[.][.]?(?:[/]|$)/.test(filePath);
}

// Example: devResolve('C:/Repos/react-native-windows/vnext/', './Libraries/Text/Text');
// Returns a full path to the resolved location which would be in the src subdirectory if
// the file exists or the directory root otherwise
function devResolve(packageName, originDir, moduleName) {
  const originDirSrc = originDir.replace(
    devPackages[packageName],
    path.join(devPackages[packageName], 'src'),
  );

  // redirect the resolution to src if an appropriate file exists there
  const extensions = [
    '',
    '.windows.tsx',
    '.windows.ts',
    '.windows.jsx',
    '.windows.js',
    '.tsx',
    '.ts',
    '.jsx',
    '.js',
  ];

  // For each potential extension we need to check for the file in either src and root
  for (const extension of extensions) {
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

/**
 * Allows the usage of live reload in packages in our repo which merges
 * Windows-specific over core. These normally work by copying from the "src"
 * subdirectory to package root during build time, but this resolver will
 * instead prefere the copy in "src" to avoid the need to build.
 */
function devResolveRequest(
  context,
  _realModuleName /* string */,
  platform /* string */,
  moduleName /* string */,
) {
  const backupResolveRequest = context.resolveRequest;
  delete context.resolveRequest;

  try {
    const modifiedModuleName =
      tryResolveDevPackage(moduleName) ||
      tryResolveDevAbsoluteImport(moduleName) ||
      tryResolveDevRelativeImport(context.originModulePath, moduleName) ||
      moduleName;
    return resolve(context, modifiedModuleName, platform);
  } catch (e) {
    throw e;
  } finally {
    context.resolveRequest = backupResolveRequest;
  }
}

function tryResolveDevPackage(moduleName) /*: string | null*/ {
  if (devPackages[moduleName]) {
    return devResolve(moduleName, devPackages[moduleName], './index');
  }

  return null;
}

function tryResolveDevAbsoluteImport(moduleName) /*: string | null*/ {
  for (const [packageName, packagePath] of Object.entries(devPackages)) {
    if (moduleName.startsWith(`${packageName}/`)) {
      return devResolve(
        packageName,
        packagePath,
        `./${moduleName.slice(`${packageName}/`.length)}`,
      );
    }
  }

  return null;
}

function tryResolveDevRelativeImport(
  originModulePath,
  moduleName,
) /*: string | null*/ {
  for (const [packageName, packagePath] of Object.entries(devPackages)) {
    if (
      isRelativeImport(moduleName) &&
      originModulePath.startsWith(packagePath)
    ) {
      const packageSrcPath = path.join(packagePath, 'src');
      const originPathWithoutSrc = originModulePath.replace(
        packageSrcPath,
        packagePath,
      );

      return devResolve(
        packageName,
        path.dirname(originPathWithoutSrc),
        moduleName,
      );
    }
  }

  return null;
}

module.exports = makeMetroConfig({
  resolver: {
    resolveRequest: devResolveRequest,
  },
});
