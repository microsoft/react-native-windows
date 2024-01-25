/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const chalk = require('chalk');
const existsSync = require('fs').existsSync;
const path = require('path');
const username = require('username');
const uuid = require('uuid');
const util = require('util');

const glob = util.promisify(require('glob'));

const templateUtils = require('../templateUtils');

async function preInstall(config = {}, options = {}) {}

async function getFileMappings(config = {}, options = {}) {
  const {rnwVersion, devMode} = templateUtils.getRnwInfo(config, options);

  const projectName =
    config?.project?.windows?.project?.projectName ?? options?.name ?? 'MyApp';
  const namespace = options?.namespace ?? projectName;
  const namespaceCpp = namespace.replace(/\./g, '::');
  const projectGuid =
    config?.project?.windows?.project?.projectGuid
      ?.replace('{', '')
      .replace('}', '') ?? uuid.v4();
  const packageGuid = uuid.v4();
  const currentUser = username.sync(); // Gets the current username depending on the platform.

  const appJsonPath = path.join(config?.root ?? process.cwd(), 'app.json');
  const mainComponentName =
    (existsSync(appJsonPath) ? require(appJsonPath).name : null) ?? projectName;

  const cppNugetPackages = [];

  const replacements = {
    useMustache: true,
    regExpPatternsToRemove: [],

    name: projectName,
    namespace: namespace,
    namespaceCpp: namespaceCpp,

    rnwVersion: rnwVersion,

    mainComponentName,

    // Visual Studio is very picky about the casing of the guids for projects, project references and the solution
    // https://www.bing.com/search?q=visual+studio+project+guid+casing&cvid=311a5ad7f9fc41089507b24600d23ee7&FORM=ANAB01&PC=U531
    // we therefore have to precariously use the right casing in the right place or risk building in VS breaking.
    projectGuidLower: `{${projectGuid.toLowerCase()}}`,
    projectGuidUpper: `{${projectGuid.toUpperCase()}}`,

    // packaging and signing variables:
    packageGuidLower: `{${packageGuid.toLowerCase()}}`,
    packageGuidUpper: `{${packageGuid.toUpperCase()}}`,
    currentUser,

    devMode,

    cppNugetPackages,
  };

  let fileMappings = [];

  const templateFiles = await glob('**/*', {
    cwd: __dirname,
    ignore: 'template.config.js',
    nodir: true,
  });

  for (const file of templateFiles) {
    const fileMapping = {
      from: path.resolve(__dirname, path.normalize(file)),
      to: path.normalize(file),
      replacements,
    };

    // Perform simple file renames
    const fileName = path.basename(fileMapping.to);
    switch (fileName) {
      case '_gitignore':
        fileMapping.to = path.join(path.dirname(fileMapping.to), '.gitignore');
        break;
      case 'NuGet_Config':
        fileMapping.to = path.join(
          path.dirname(fileMapping.to),
          'NuGet.config',
        );
        break;
    }

    // Rename files with MyApp in the name
    fileMapping.to = fileMapping.to.replace(/MyApp/g, projectName);

    fileMappings.push(fileMapping);
  }

  return fileMappings;
}

async function postInstall(config = {}, options = {}) {
  // Update package.json with new scripts and dependencies
  await templateUtils.updateProjectPackageJson(config, options, {
    scripts: {
      windows: 'react-native run-windows',
      'test:windows': 'jest --config jest.config.windows.js',
    },
    devDependencies: {
      '@rnx-kit/jest-preset': '^0.1.16',
    },
  });

  // Install recently added dependencies
  await templateUtils.runNpmInstall(config, options);

  console.log(chalk.white.bold('To run your new windows app:'));
  console.log(chalk.white('   npx react-native run-windows'));
}

module.exports = {
  name: 'React Native Windows Application (New Arch, C++, Win32, Hermes)',
  description:
    "[Experimental] A RNW app targeting RN's new architecture, with the Hermes JS engine.",
  preInstall,
  getFileMappings,
  postInstall,
};
