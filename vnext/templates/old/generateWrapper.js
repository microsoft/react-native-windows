/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const generateWindows = require('../../generate');
const templateUtils = require('../templateUtils');

function makeGenerateWindowsWrapper(language = 'cpp', isDefault = false) {
  const name = `React Native Windows Application (Old Arch, UWP, ${
    language === 'cs' ? 'C#' : 'C++'
  })`;
  const description = `[Legacy] A RNW app using RN's Old Architecture, built in ${
    language === 'cs' ? 'C#' : 'C++'
  } and targeting UWP.`;

  const postInstall = async (config = {}, options = {}) => {
    const experimentalFeatures = config?.project?.windows?.experimentalFeatures;

    const rnwInfo = templateUtils.getRnwInfo(config, options);

    const generateOptions = {
      overwrite: !!options.overwrite,
      language,
      projectType: 'app',
      experimentalNuGetDependency:
        experimentalFeatures?.UseExperimentalNuget === 'true' ?? false,
      useWinUI3: experimentalFeatures?.UseWinUI3 === 'true' ?? false,
      useHermes: experimentalFeatures?.UseHermes === 'true' ?? true,
      useDevMode: rnwInfo.devMode,
      verbose: !!options.logging,
      telemetry: !!options.telemetry,
    };

    await generateWindows(
      config.root,
      options.name,
      options.namespace,
      generateOptions,
    );

    await templateUtils.updateProjectPackageJson(config, options);
  };

  return {
    name,
    description,
    isDefault,
    postInstall,
  };
}

module.exports = {
  makeGenerateWindowsWrapper,
};
