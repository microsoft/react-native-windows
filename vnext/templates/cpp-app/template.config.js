/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const chalk = require('chalk');
const existsSync = require('fs').existsSync;
const fs = require('fs').promises;
const path = require('path');
const username = require('username');
const uuid = require('uuid');

async function preInstall(config = {}, options = {}) {}

async function getFileMappings(config = {}, options = {}) {
  const rnwPath = path.dirname(
    require.resolve('react-native-windows', [config.root]),
  );
  const rnwVersion = require(path.join(rnwPath, 'package.json')).version;

  const devMode = await existsSync(path.join(rnwPath, 'src'));

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

  const cppNugetPackages = [
    {id: 'boost', version: '1.76.0.0'},
    {
      id: 'Microsoft.Toolkit.Win32.UI.XamlApplication',
      version: '6.1.3',
    },
    {
      id: 'Microsoft.VCRTForwarders.140',
      version: '1.0.2-rc',
    },
  ];

  const replacements = {
    useMustache: true,
    regExpPatternsToRemove: [],

    name: projectName,
    namespace: namespace,
    namespaceCpp: namespaceCpp,

    rnwVersion: rnwVersion,

    mainComponentName: projectName, // TODO: replace with app.json name

    // Visual Studio is very picky about the casing of the guids for projects, project references and the solution
    // https://www.bing.com/search?q=visual+studio+project+guid+casing&cvid=311a5ad7f9fc41089507b24600d23ee7&FORM=ANAB01&PC=U531
    // we therefore have to precariously use the right casing in the right place or risk building in VS breaking.
    projectGuidLower: `{${projectGuid.toLowerCase()}}`,
    projectGuidUpper: `{${projectGuid.toUpperCase()}}`,

    // packaging and signing variables:
    packageGuidLower: `{${packageGuid.toLowerCase()}}`,
    packageGuidUpper: `{${packageGuid.toUpperCase()}}`,
    currentUser: currentUser,

    devMode,

    cppNugetPackages,
  };

  let fileMappings = [];

  const templateFiles = await fs.readdir(__dirname, {recursive: true});

  for (const file of templateFiles) {
    const fileMapping = {
      from: path.resolve(__dirname, file),
      to: file,
      replacements,
    };

    // Don't include this file nor directories
    if (
      file === 'template.config.js' ||
      (await fs.stat(fileMapping.from)).isDirectory()
    ) {
      continue;
    }

    // Perform simple file renames
    const fileName = path.basename(fileMapping.to);
    switch (fileName) {
      case '_gitignore':
        fileMapping.to = path.join(path.dirname(file), '.gitignore');
        break;
      case 'NuGet_Config':
        fileMapping.to = path.join(path.dirname(file), 'NuGet.config');
        break;
    }

    // Rename files with MyApp in the name
    fileMapping.to = fileMapping.to.replace(/MyApp/g, projectName);

    fileMappings.push(fileMapping);
  }

  return fileMappings;
}

function postInstall(config = {}, options = {}) {
  console.log(chalk.white.bold('To run your app on UWP:'));
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
