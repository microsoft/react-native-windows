/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const execSync = require('child_process').execSync;
const fs = require('fs');
const path = require('path');
const chalk = require('chalk');
const performance = require('perf_hooks').performance;

const {newSpinner} = require('./commandWithProgress');
const vstools = require('./vstools');
const generatorCommon = require('../../generator-common');

const templateRoot = path.join(__dirname, '../../generator-windows/templates');

/**
 * Logs the given message if verbose is True.
 * @param {string} message The message to log.
 * @param {boolean} verbose Whether or not verbose logging is enabled.
 */
function verboseMessage(message, verbose) {
  if (verbose) {
    console.log(message);
  }
}

/**
 * Loads a source template file and performs the given replacements, normalizing CRLF.
 * @param {string} srcFile Path to the source file.
 * @param {object} replacements e.g. {'TextToBeReplaced': 'Replacement'}
 * @return The contents of the file with the replacements applied.
 */
function getNormalizedContents(srcFile, replacements) {
  // Template files are CRLF, JS-generated replacements are LF, normalize replacements to CRLF
  for (var key in replacements) {
    replacements[key] = replacements[key].replace(/\n/g, '\r\n');
  }

  return generatorCommon.resolveContents(srcFile, replacements);
}

/**
 * Updates the target file with the expected contents if it's different.
 * @param {string} filePath Path to the target file to update.
 * @param {string} expectedContents The expected contents of the file.
 * @param {boolean} verbose If true, enable verbose logging.
 * @param {boolean} checkMode It true, don't make any changes.
 * @return {boolean} Whether any changes were necessary.
 */
function updateFile(filePath, expectedContents, verbose, checkMode) {
  const fileName = chalk.bold(path.basename(filePath));
  verboseMessage(`Reading ${fileName}...`, verbose);
  const actualContents = fs.existsSync(filePath)
    ? fs.readFileSync(filePath).toString()
    : '';

  const contentsChanged = expectedContents !== actualContents;

  if (contentsChanged) {
    verboseMessage(chalk.yellow(`${fileName} needs to be updated.`), verbose);
    if (!checkMode) {
      verboseMessage(`Writing ${fileName}...`, verbose);
      fs.writeFileSync(filePath, expectedContents, {
        encoding: 'utf8',
        flag: 'w',
      });
    }
  } else {
    verboseMessage(`No changes to ${fileName}.`, verbose);
  }

  return contentsChanged;
}

/**
 * Exits the script with the given status code.
 * @param {number} statusCode The status code.
 * @param {boolean} loggingWasEnabled Whether or not verbose lossing was enabled.
 */
function exitProcessWithStatusCode(statusCode, loggingWasEnabled) {
  if (!loggingWasEnabled && statusCode !== 0) {
    console.log(
      `Error: Re-run the command with ${chalk.bold(
        '--logging',
      )} for more information.`,
    );
  }
  process.exit(statusCode);
}

/**
 * Performs auto-linking for RNW native modules and apps.
 * @param {object} config Config passed from react-native CLI.
 * @param {object} args Args passed from react-native CLI.
 * @param {object} options Options passed from react-native CLI.
 */
async function updateAutoLink(config, args, options) {
  const startTime = performance.now();

  const verbose = options.logging;

  const checkMode = options.check;

  var changesNecessary = false;

  const spinner = newSpinner(
    checkMode ? 'Checking auto-linked files...' : 'Auto-linking...',
  );

  verboseMessage('', verbose);

  const execString = 'react-native config';
  let output;
  try {
    verboseMessage(`Running ${chalk.bold('react-native config')}...`, verbose);

    output = execSync(execString).toString();

    verboseMessage(output, verbose);

    verboseMessage('Parsing output...', verbose);

    const rnConfig = JSON.parse(output);
    const windowsAppConfig = rnConfig.project.windows;

    if (!windowsAppConfig) {
      throw new Error(
        'Windows auto-link only supported on windows app projects.',
      );
    }

    verboseMessage('Found Windows app project, parsing...', verbose);

    const solutionFile = path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      windowsAppConfig.solutionFile,
    );

    const projectFile = path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      windowsAppConfig.project.projectFile,
    );

    const projectDir = path.dirname(projectFile);
    const projectLang = windowsAppConfig.project.projectLang;

    verboseMessage('Parsing dependencies...', verbose);

    const dependencies = rnConfig.dependencies;

    let windowsDependencies = {};

    for (const dependencyName in dependencies) {
      const windowsDependency = dependencies[dependencyName].platforms.windows;

      if (windowsDependency) {
        verboseMessage(
          `Found dependency ${chalk.bold(dependencyName)}.`,
          verbose,
        );
        windowsDependencies[dependencyName] = windowsDependency;
      }
    }

    // Generating cs/h files for app code consumption
    if (projectLang === 'cs') {
      let csUsingNamespaces = '';
      let csReactPacakgeProviders = '';

      for (const dependencyName in windowsDependencies) {
        windowsDependencies[dependencyName].projects.forEach(project => {
          if (project.directDependency) {
            csUsingNamespaces += `\n\n// Namespaces from ${dependencyName}`;
            project.csNamespaces.forEach(namespace => {
              csUsingNamespaces += `\nusing ${namespace};`;
            });

            csReactPacakgeProviders += `\n            // IReactPackageProviders from ${dependencyName}`;
            project.csPackageProviders.forEach(packageProvider => {
              csReactPacakgeProviders += `\n            packageProviders.Add(new ${packageProvider}());`;
            });
          }
        });
      }

      const csFileName = 'AutolinkedNativeModules.g.cs';

      const srcCsFile = path.join(templateRoot, projectLang, 'src', csFileName);

      const destCsFile = path.join(projectDir, csFileName);

      verboseMessage(
        `Calculating ${chalk.bold(path.basename(destCsFile))}...`,
        verbose,
      );

      const csContents = getNormalizedContents(srcCsFile, {
        '<%=AutolinkCsUsingNamespaces%>': csUsingNamespaces,
        '<%=AutolinkCsReactPacakgeProviders%>': csReactPacakgeProviders,
      });

      changesNecessary =
        updateFile(destCsFile, csContents, verbose, checkMode) ||
        changesNecessary;
    } else if (projectLang === 'cpp') {
      let cppIncludes = '';
      let cppPackageProviders = '';

      for (const dependencyName in windowsDependencies) {
        windowsDependencies[dependencyName].projects.forEach(project => {
          if (project.directDependency) {
            cppIncludes += `\n\n// Includes from ${dependencyName}`;
            project.cppHeaders.forEach(header => {
              cppIncludes += `\n#include <${header}>`;
            });

            cppPackageProviders += `\n    // IReactPackageProviders from ${dependencyName}`;
            project.cppPackageProviders.forEach(packageProvider => {
              cppPackageProviders += `\n    packageProviders.Append(winrt::${packageProvider}());`;
            });
          }
        });
      }

      const cppFileName = 'AutolinkedNativeModules.g.cpp';

      const srcCppFile = path.join(
        templateRoot,
        projectLang,
        'src',
        cppFileName,
      );

      const destCppFile = path.join(projectDir, cppFileName);

      verboseMessage(
        `Calculating ${chalk.bold(path.basename(destCppFile))}...`,
        verbose,
      );

      const cppContents = getNormalizedContents(srcCppFile, {
        '<%=AutolinkCppIncludes%>': cppIncludes,
        '<%=AutolinkCppPackageProviders%>': cppPackageProviders,
      });

      changesNecessary =
        updateFile(destCppFile, cppContents, verbose, checkMode) ||
        changesNecessary;
    }

    // Generating targets for app project consumption
    let projectReferencesForTargets = '';

    for (const dependencyName in windowsDependencies) {
      windowsDependencies[dependencyName].projects.forEach(project => {
        if (project.directDependency) {
          const dependencyProjectFile = path.join(
            windowsDependencies[dependencyName].folder,
            windowsDependencies[dependencyName].sourceDir,
            project.projectFile,
          );

          const relDependencyProjectFile = path.relative(
            projectDir,
            dependencyProjectFile,
          );

          projectReferencesForTargets += `\n    <!-- Projects from ${dependencyName} -->`;
          projectReferencesForTargets += `\n    <ProjectReference Include="$(ProjectDir)${relDependencyProjectFile}">
      <Project>${project.projectGuid}</Project>
    </ProjectReference>`;
        }
      });
    }

    const targetFileName = 'AutolinkedNativeModules.g.targets';

    const srcTargetFile = path.join(
      templateRoot,
      projectLang,
      'src',
      targetFileName,
    );

    const destTargetFile = path.join(projectDir, targetFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destTargetFile))}...`,
      verbose,
    );

    const targetContents = getNormalizedContents(srcTargetFile, {
      '<%=AutolinkProjectReferencesForTargets%>': projectReferencesForTargets,
    });

    changesNecessary =
      updateFile(destTargetFile, targetContents, verbose, checkMode) ||
      changesNecessary;

    // Generating project entries for solution
    let projectsForSolution = [];

    for (const dependencyName in windowsDependencies) {
      // Process projects
      windowsDependencies[dependencyName].projects.forEach(project => {
        const dependencyProjectFile = path.join(
          windowsDependencies[dependencyName].folder,
          windowsDependencies[dependencyName].sourceDir,
          project.projectFile,
        );

        projectsForSolution.push({
          projectFile: dependencyProjectFile,
          projectName: project.projectName,
          projectLang: project.projectLang,
          projectGuid: project.projectGuid,
        });
      });
    }

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(solutionFile))} changes...`,
      verbose,
    );

    projectsForSolution.forEach(project => {
      const contentsChanged = vstools.addProjectToSolution(
        solutionFile,
        project,
        verbose,
        checkMode,
      );
      changesNecessary = changesNecessary || contentsChanged;
    });

    spinner.succeed();
    var endTime = performance.now();

    if (!changesNecessary) {
      console.log(
        `${chalk.green(
          'Success:',
        )} No auto-linking changes necessary. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    } else if (checkMode) {
      console.log(
        `${chalk.yellow(
          'Warning:',
        )} Auto-linking changes were necessary but ${chalk.bold(
          '--check',
        )} specified. Run ${chalk.bold(
          "'npx react-native autolink-windows'",
        )} to apply the changes. (${Math.round(endTime - startTime)}ms)`,
      );
      exitProcessWithStatusCode(0, verbose);
    } else {
      console.log(
        `${chalk.green(
          'Success:',
        )} Auto-linking changes completed. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    }
  } catch (e) {
    spinner.fail();
    var endTime = performance.now();
    console.log(
      `${chalk.red('Error:')} ${e.toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    exitProcessWithStatusCode(1, verbose);
  }
}

module.exports = {
  name: 'autolink-windows',
  description: 'performs autolinking',
  func: updateAutoLink,
  options: [
    {
      command: '--logging',
      description: 'Verbose output logging',
      default: false,
    },
    {
      command: '--check',
      description: 'Only check whether any autolinked files need to change',
      default: false,
    },
  ],
};
