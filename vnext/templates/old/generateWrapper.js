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
      ? `React Native Windows Library (Paper, ${
          language === 'cs' ? 'C#' : 'C++'
        }, UWP)`
      : `React Native Windows Application (Paper, ${
          language === 'cs' ? 'C#' : 'C++'
        }, UWP, Chakra)`;
  const description =
    projectType === 'lib'
      ? `A UWP module written in ${
          language === 'cs' ? 'C#' : 'C++'
        }, using the Paper architecture.`
      : `A UWP app written in ${
          language === 'cs' ? 'C#' : 'C++'
        }, using the Paper architecture and defaulting to the Chakra JS engine.`;

  const postInstall = async (config = {}, options = {}) => {
    const experimentalFeatures = config?.project?.windows?.experimentalFeatures;

    const generateOptions = {
      overwrite: !!options.overwrite,
      language,
      projectType,
      experimentalNuGetDependency:
        experimentalFeatures?.UseExperimentalNuget ?? false,
      useWinUI3: experimentalFeatures?.UseWinUI3 ?? false,
      useHermes: experimentalFeatures?.UseHermes ?? false,
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
