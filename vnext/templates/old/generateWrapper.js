/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const generateWindows = require('../../generate');

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

    const generateOptions = {
      overwrite: !!options.overwrite,
      language,
      projectType,
      experimentalNuGetDependency:
        experimentalFeatures?.UseExperimentalNuget ?? false,
      useWinUI3: experimentalFeatures?.UseWinUI3 ?? false,
      useHermes: experimentalFeatures?.UseHermes ?? true,
      useDevMode: false,
      verbose: !!options.logging,
      telemetry: !!options.telemetry,
    };

    await generateWindows(
      config.root,
      options.name,
      options.namespace,
      generateOptions,
    );
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
