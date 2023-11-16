/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const pkgUtils = require('@react-native-windows/package-utils');

async function updateProjectPackageJson(config = {}, options = {}, props = {}) {
  const projectPath = config?.root ?? process.cwd();
  const projectPackage = await pkgUtils.WritableNpmPackage.fromPath(
    projectPath,
  );

  if (!projectPackage) {
    throw new Error(
      `The directory '${projectPath}' is not the root of an npm package`,
    );
  }

  if (options?.logging) {
    console.log('Modifying project package.json...');
  }
  await projectPackage.mergeProps(props);
}

module.exports = {updateProjectPackageJson};
