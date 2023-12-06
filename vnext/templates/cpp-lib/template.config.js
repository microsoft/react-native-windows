/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const existsSync = require('fs').existsSync;
const path = require('path');
const username = require('username');
const uuid = require('uuid');
const util = require('util');

const glob = util.promisify(require('glob'));

const templateUtils = require('../templateUtils');

function getExampleInfo(config = {}, options = {}) {
  const projectPath = config?.root ?? process.cwd();
  const exampleProjectPath = path.join(projectPath, 'example');

  const exists = existsSync(exampleProjectPath);
  const exProjectConfig = exists
    ? templateUtils.getWindowsProjectConfig(exampleProjectPath)
    : null;
  const exOptions = exists
    ? {
        ...options,
        template: 'cpp-app',
        name: `${options?.name ?? 'MyLib'}Example`,
        namespace: `${options?.namespace ?? 'MyLib'}Example`,
      }
    : null;

  return {
    exists,
    config: {
      root: exampleProjectPath,
      project: {
        windows: exProjectConfig,
      },
    },
    options: exOptions,
  };
}

const exampleTemplateConfig = require('../cpp-app/template.config');

async function preInstall(config = {}, options = {}) {
  const exampleInfo = getExampleInfo(config, options);

  if (exampleInfo.exists) {
    if (options?.logging) {
      console.log('Running cpp-app template preInstall() for example...');
    }
    await exampleTemplateConfig.preInstall(
      exampleInfo.config,
      exampleInfo.options,
    );
  }
}

async function getFileMappings(config = {}, options = {}) {
  const {rnwVersion, devMode} = templateUtils.getRnwInfo(config, options);

  const projectName =
    config?.project?.windows?.project?.projectName ?? options?.name ?? 'MyLib';
  const namespace = options?.namespace ?? projectName;
  const namespaceCpp = namespace.replace(/\./g, '::');
  const projectGuid =
    config?.project?.windows?.project?.projectGuid
      ?.replace('{', '')
      .replace('}', '') ?? uuid.v4();
  const currentUser = username.sync(); // Gets the current username depending on the platform.

  const cppNugetPackages = [];

  const replacements = {
    useMustache: true,
    regExpPatternsToRemove: [],

    name: projectName,
    pascalName: templateUtils.pascalCase(projectName),
    namespace: namespace,
    namespaceCpp: namespaceCpp,

    rnwVersion: rnwVersion,

    // Visual Studio is very picky about the casing of the guids for projects, project references and the solution
    // https://www.bing.com/search?q=visual+studio+project+guid+casing&cvid=311a5ad7f9fc41089507b24600d23ee7&FORM=ANAB01&PC=U531
    // we therefore have to precariously use the right casing in the right place or risk building in VS breaking.
    projectGuidLower: `{${projectGuid.toLowerCase()}}`,
    projectGuidUpper: `{${projectGuid.toUpperCase()}}`,

    currentUser: currentUser,

    devMode,

    cppNugetPackages,
  };

  let fileMappings = [];

  const templateFiles = await glob('**/*', {
    cwd: __dirname,
    ignore: 'template.config.js',
    nodir: true,
  });

  const exampleInfo = getExampleInfo(config, options);

  for (const file of templateFiles) {
    const fileMapping = {
      from: path.resolve(__dirname, path.normalize(file)),
      to: path.normalize(file),
      replacements,
    };

    // Don't copy example files if there is no example in the destination
    if (!exampleInfo.exists && fileMapping.to.startsWith('example')) {
      continue;
    }

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

    // Rename files with MyLib in the name
    fileMapping.to = fileMapping.to.replace(/MyLib/g, projectName);

    fileMappings.push(fileMapping);
  }

  // Add the file mappings from the cpp-app template for the example app
  if (exampleInfo.exists) {
    const exampleFileMappings = await exampleTemplateConfig.getFileMappings(
      exampleInfo.config,
      exampleInfo.options,
    );

    for (const exFileMap of exampleFileMappings) {
      exFileMap.to = path.join('example', exFileMap.to);

      // Only add the file map if there isn't a mapping from this cpp-lib template
      if (fileMappings.filter(fm => fm.to === exFileMap.to).length === 0) {
        fileMappings.push(exFileMap);
      }
    }
  }

  return fileMappings;
}

async function postInstall(config = {}, options = {}) {
  const projectName =
    config?.project?.windows?.project?.projectName ?? options?.name ?? 'MyLib';
  const namespace = options?.namespace ?? projectName;
  const namespaceCpp = namespace.replace(/\./g, '::');

  // Update package.json codegen
  await templateUtils.updateProjectPackageJson(config, options, {
    codegenConfig: {
      windows: {
        namespace: namespaceCpp + 'Codegen',
        outputDirectory: `windows/${projectName}/codegen`,
        separateDataTypes: true,
      },
    },
  });

  // Fix babel config's out of date preset
  await templateUtils.replaceInFile(
    config,
    options,
    'babel.config.js',
    'module:metro-react-native-babel-preset',
    'module:@react-native/babel-preset',
  );

  const exampleInfo = getExampleInfo(config, options);

  if (exampleInfo.exists) {
    const {rnwVersion} = templateUtils.getRnwInfo(
      exampleInfo.config,
      exampleInfo.options,
    );

    // Update example package.json with new scripts and dependencies
    await templateUtils.updateProjectPackageJson(
      exampleInfo.config,
      exampleInfo.options,
      {
        scripts: {
          windows: 'react-native run-windows',
          'test:windows': 'jest --config jest.config.windows.js',
        },
        dependencies: {
          'react-native-windows': rnwVersion,
        },
        devDependencies: {
          '@rnx-kit/jest-preset': '^0.1.16',
        },
      },
    );

    // Fix babel config's out of date preset
    await templateUtils.replaceInFile(
      exampleInfo.config,
      exampleInfo.options,
      'babel.config.js',
      'module:metro-react-native-babel-preset',
      'module:@react-native/babel-preset',
    );

    // Install recently added dependencies (doesn't work, uses wrong yarn)
    //await templateUtils.runNpmInstall(exampleInfo.config, exampleInfo.options);

    console.log(
      'Run yarn to install new dependencies for the example project.',
    );
  }
}

module.exports = {
  name: 'React Native Windows Turbo Module (New Arch, C++)',
  description:
    "[Experimental] A RNW turbo module targeting RN's new architecture.",
  preInstall,
  getFileMappings,
  postInstall,
};
