/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const generateWindows = require('../../generate');
const templateUtils = require('../templateUtils');

function makeGenerateWindowsWrapper(
  language = 'cpp',
  projectType = 'app',
  isDefault = false,
) {
  const name =
    projectType === 'lib'
      ? `React Native Windows Library (Old Arch, UWP, ${
          language === 'cs' ? 'C#' : 'C++'
        })`
      : `React Native Windows Application (Old Arch, UWP, ${
          language === 'cs' ? 'C#' : 'C++'
        }, Hermes)`;
  const description =
    projectType === 'lib'
      ? `A RNW module written in ${
          language === 'cs' ? 'C#' : 'C++'
        }, targeting UWP and RN's old architecture.`
      : `A RNW app written in ${
          language === 'cs' ? 'C#' : 'C++'
        }, targeting UWP and RN's old architecture, with the Hermes JS engine.`;

  const postInstall = async (config = {}, options = {}) => {
    const experimentalFeatures = config?.project?.windows?.experimentalFeatures;

    const rnwInfo = templateUtils.getRnwInfo(config, options);

    const generateOptions = {
      overwrite: !!options.overwrite,
      language,
      projectType,
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
