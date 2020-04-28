/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const execSync = require('child_process').execSync;
const path = require('path');
const fs = require('fs');

function updateAutoLink(verbose) {
  const execString = 'react-native config';
  let output;
  try {
    console.log('Running react-native config...');
    output = execSync(execString).toString();
    if (verbose) {
      console.log(output);
    }

    const config = JSON.parse(output);
    const windowsPlatformConfig = config.project.windows;
    const cppProjFile = windowsPlatformConfig
      ? windowsPlatformConfig.cppProjFile
      : null;
    if (cppProjFile == null) {
      console.log(
        'AutoLink currently is only supported on C++/WinRT main project.',
      );
      return;
    }
    console.log(`cppProjFile: ${cppProjFile}`);

    const sourceDir = config.project.windows.sourceDir;
    console.log(`sourceDir: ${sourceDir}`);

    const projectName = config.project.windows.projectName;
    console.log(`projectName: ${projectName}`);

    //#1. update nativeModules.g.h
    const generatedHeader = path.join(
      sourceDir,
      projectName,
      'AutolinkedNativeModules.g.h',
    );
    if (!fs.existsSync(generatedHeader)) {
      console.log(
        'AutoLink can not locate generated header file: {generatedHeader}',
      );
      return;
    }

    let generatedIncludes =
      '// AutolinkedNativeModules.g.h -- contents generated by "react-native run-windows"\\\r\n\\\r\n#pragma once\\\r\n\\\r\n';
    const dependencies = config.dependencies;
    let packageRegistrations =
      '#define REGISTER_AUTOLINKED_NATIVE_MODULE_PACKAGES()';
    for (const dependency in dependencies) {
      const windowDependency = dependencies[dependency].platforms.windows;
      const cppProjFile = windowDependency
        ? windowDependency.cppProjFile
        : null;
      if (cppProjFile == null) {
        console.log('No C++/WinRT project found for ' + dependency);
        continue;
      }
      console.log(
        `Adding include and package provider statement for ${dependency}`,
      );
      const trimmedPackageName = windowDependency.packageName.trim();
      const includeStatement = `\r\n#include <winrt/${trimmedPackageName}.h>`;
      generatedIncludes += includeStatement;
      const packageRegistration = `    \\\r\n    PackageProviders().Append(winrt::${trimmedPackageName}::ReactPackageProvider());`;
      packageRegistrations += packageRegistration;
    }

    console.log('Updating AutolinkedNativeModules.g.h...');
    const contents = generatedIncludes + '\r\n' + packageRegistrations;
    fs.writeFileSync(generatedHeader, contents, {encoding: 'utf8', flag: 'w'});

    //TODO:
    //#2. Update project file to add references to native module packages

    //TODO:
    //#3. Update solution file to include native module project files

    return;
  } catch (e) {
    console.error('Parsing react-native config failed!');
    console.error(e);
    return;
  }
}

module.exports = {
  updateAutoLink,
};
