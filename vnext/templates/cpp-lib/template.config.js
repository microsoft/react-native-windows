/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const crypto = require('crypto');
const existsSync = require('fs').existsSync;
const path = require('path');
const username = require('username');
const util = require('util');

const glob = util.promisify(require('glob'));

const templateUtils = require('../templateUtils');

function resolveArgs(config = {}, options = {}) {
  const projectRoot = config?.root ?? process.cwd();

  const libConfig = {...config};
  const libOptions = {...options};

  if (libConfig.project?.windows?.project?.projectFile?.startsWith('Error:')) {
    libConfig.project.windows =
      templateUtils.getWindowsDependencyConfig(projectRoot);
  }

  const exampleProjectPath = path.join(projectRoot, 'example');

  const exExists = existsSync(exampleProjectPath);
  const exProjectConfig = exExists
    ? templateUtils.getWindowsProjectConfig(exampleProjectPath)
    : null;
  const exOptions = exExists
    ? {
        ...options,
        template: 'cpp-app',
        name: `${options?.name ?? 'MyLib'}Example`,
        namespace: `${options?.namespace ?? 'MyLib'}Example`,
      }
    : null;

  return {
    libConfig,
    libOptions,
    exExists,
    exConfig: {
      root: exampleProjectPath,
      project: {
        windows: exProjectConfig,
      },
    },
    exOptions,
  };
}

const exampleTemplateConfig = require('../cpp-app/template.config');

async function preInstall(config = {}, options = {}) {
  const {exExists, exConfig, exOptions} = resolveArgs(config, options);

  if (exExists) {
    if (exOptions?.logging) {
      console.log('Running cpp-app template preInstall() for example...');
    }
    await exampleTemplateConfig.preInstall(exConfig, exOptions);
  }
}

async function getFileMappings(config = {}, options = {}) {
  const {libConfig, libOptions, exExists, exConfig, exOptions} = resolveArgs(
    config,
    options,
  );

  const projectRoot = libConfig.root ?? process.cwd();
  const {rnwPath, rnwVersion, devMode, isCanary} = templateUtils.getRnwInfo(
    libConfig,
    libOptions,
  );

  const projectName =
    libConfig?.project?.windows?.projects[0]?.projectName ??
    libOptions?.name ??
    'MyLib';
  const namespace = libOptions?.namespace ?? projectName;
  const namespaceCpp = namespace.replace(/\./g, '::');
  const projectGuid =
    libConfig?.project?.windows?.projects[0]?.projectGuid
      ?.replace('{', '')
      .replace('}', '') ?? crypto.randomUUID();
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
    rnwPathFromProjectRoot: path
      .relative(projectRoot, rnwPath)
      .replace(/\//g, '\\'),

    // Visual Studio is very picky about the casing of the guids for projects, project references and the solution
    // https://www.bing.com/search?q=visual+studio+project+guid+casing&cvid=311a5ad7f9fc41089507b24600d23ee7&FORM=ANAB01&PC=U531
    // we therefore have to precariously use the right casing in the right place or risk building in VS breaking.
    projectGuidLower: `{${projectGuid.toLowerCase()}}`,
    projectGuidUpper: `{${projectGuid.toUpperCase()}}`,

    currentUser,

    devMode,

    useNuGets: !devMode, // default is to use published NuGets except in devMode, change to true here if you want to test devMode and nugets simultaneously
    addReactNativePublicAdoFeed: isCanary,

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

    // Don't copy example files if there is no example in the destination
    if (!exExists && fileMapping.to.startsWith('example')) {
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
  if (exExists) {
    const exampleFileMappings = await exampleTemplateConfig.getFileMappings(
      exConfig,
      exOptions,
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
  const {libConfig, libOptions, exExists, exConfig, exOptions} = resolveArgs(
    config,
    options,
  );

  const projectName =
    libConfig?.project?.windows?.projects[0]?.projectName ??
    libOptions?.name ??
    'MyLib';
  const namespace = libOptions?.namespace ?? projectName;
  const namespaceCpp = namespace.replace(/\./g, '::');

  // Update package.json codegen
  await templateUtils.updateProjectPackageJson(libConfig, libOptions, {
    codegenConfig: {
      windows: {
        namespace: namespaceCpp + 'Codegen',
        outputDirectory: `windows/${projectName}/codegen`,
        separateDataTypes: true,
      },
    },
  });

  if (exExists) {
    const {rnwVersion} = templateUtils.getRnwInfo(exConfig, exOptions);

    // Update example package.json with new scripts and dependencies
    await templateUtils.updateProjectPackageJson(exConfig, exOptions, {
      scripts: {
        windows: 'npx @react-native-community/cli run-windows',
        'test:windows': 'jest --config jest.config.windows.js',
      },
      dependencies: {
        'react-native-windows': rnwVersion,
      },
      devDependencies: {
        '@rnx-kit/jest-preset': '^0.1.17',
      },
    });

    // Install recently added dependencies
    await templateUtils.runNpmInstall(libConfig, libOptions);

    console.log(
      '\nRun the example app on Windows:\n\n  > yarn example windows\n',
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
