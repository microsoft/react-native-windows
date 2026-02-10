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
  const currentUser = username.sync();

  const pkgJsonPath = path.join(projectRoot, 'package.json');
  if (!existsSync(pkgJsonPath)) {
    throw new Error('package.json not found in user library root');
  }

  const pkgJson = require(pkgJsonPath);

  if (!pkgJson.codegenConfig?.name) {
    throw new Error(
      'codegenConfig.name is required in package.json for C++ libraries',
    );
  }

  const codegenName = pkgJson.codegenConfig.name;

  const replacements = {
    useMustache: true,
    regExpPatternsToRemove: [],

    name: projectName,
    pascalName: templateUtils.pascalCase(projectName),
    namespace,
    namespaceCpp,
    codegenName,

    rnwVersion,
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
    addReactNativePublicAdoFeed: true || isCanary, // Temporary true for all new projects until code-signing is restored, see issue #14030

    cppNugetPackages: [],
  };

  let fileMappings = [];

  const templateFiles = templateUtils.getTemplateFiles(__dirname);

  for (const file of templateFiles) {
    const fileMapping = {
      from: file.from,
      to: file.to.replace(/MyLib/g, projectName),
      replacements,
    };

    // Don't copy example files if there is no example in the destination
    if (!exExists && fileMapping.to.startsWith('example')) {
      continue;
    }

    fileMappings.push(fileMapping);
  }

  if (exExists) {
    const exampleFileMappings = await exampleTemplateConfig.getFileMappings(
      exConfig,
      exOptions,
    );

    for (const exFileMap of exampleFileMappings) {
      exFileMap.to = path.join('example', exFileMap.to);
      fileMappings.push(exFileMap);
    }
  }

  return fileMappings;
}

async function postInstall(config = {}, options = {}) {
  const {libConfig, libOptions} = resolveArgs(config, options);

  const projectName =
    libConfig?.project?.windows?.projects[0]?.projectName ??
    libOptions?.name ??
    'MyLib';

  const namespace = libOptions?.namespace ?? projectName;
  const namespaceCpp = namespace.replace(/\./g, '::');

  // Update package.json codegen
  await templateUtils.updateProjectPackageJson(
    libConfig,
    libOptions,
    {
      codegenConfig: {
        windows: {
          namespace: `${namespaceCpp}Codegen`,
          outputDirectory: `windows/${projectName}/codegen`,
          separateDataTypes: true,
        },
      },
    },
    true, // save options from command
    true, // if a "files" property exists, make sure "windows" is included
  );
}

module.exports = {
  name: 'React Native Windows Library (C++)',
  description:
    "A RNW (Turbo) Native Module supporting RN's New and Old Architectures built in C++.",
  preInstall,
  getFileMappings,
  postInstall,
};
