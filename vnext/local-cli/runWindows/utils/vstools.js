/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const fs = require('fs');
const path = require('path');
const chalk = require('chalk');

const projectTypeGuidsByLanguage = {
  cpp: '{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}',
  cs: '{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}',
};

/**
 * Checks is the given block of lines exists within an array of lines.
 * @param {array} lines - The array of lines to search.
 * @param {array} block - The block of lines to search for.
 * @return {boolean} - True if the block of lines does exist within lines.
 */
function linesContainsBlock(lines, block) {
  if (block.length > 0) {
    var startIndex = lines.indexOf(block[0]);

    if (startIndex >= 0) {
      for (let i = 1; i < block.length; i++) {
        if (lines[startIndex + i] !== block[i]) {
          return false;
        }
      }
      return true;
    }
  }

  return false;
}

/**
 * Insert the given block of lines into an array of lines.
 * @param {array} lines - The array of lines to insert into.
 * @param {array} block - The block of lines to insert.
 * @param {number} index - The index to perform the insert.
 */
function insertBlockIntoLines(lines, block, index) {
  for (let i = 0; i < block.length; i++) {
    lines.splice(index + i, 0, block[i]);
  }
}

/**
 * Search through an array of lines for a block of lines starting with startLine and ending with endLine.
 * @param {array} lines - The array of lines to search.
 * @param {string} startLine - The first line of the block.
 * @param {string} endLine - The last line of the block.
 * @param {boolean} includeStartEnd - Include the start and end lines in the result.
 * @return {array} The found block of lines, if found.
 */
function getBlockContentsFromLines(
  lines,
  startLine,
  endLine,
  includeStartEnd = true,
) {
  const startIndex = lines.indexOf(startLine);
  const endIndex = lines.indexOf(endLine, startIndex);

  if (startIndex >= 0 && startIndex < endIndex) {
    if (includeStartEnd) {
      return lines.slice(startIndex, endIndex + 1);
    } else if (startIndex + 1 < endIndex) {
      return lines.slice(startIndex + 1, endIndex);
    }
  }

  return [];
}

/**
 * Adds the necessary info from a VS project into a VS solution file so that it will build.
 * @param {string} slnFile - The Absolute path to the target VS solution file.
 * @param {object} project - The object representing the project info.
 * @param {boolean} verbose - If true, enable verbose logging.
 * @param {boolean} checkMode - It true, don't make any changes.
 * @return {boolean} Whether any changes were necessary.
 */
function addProjectToSolution(
  slnFile,
  project,
  verbose = false,
  checkMode = false,
) {
  if (verbose) {
    console.log(
      `Processing ${chalk.bold(path.basename(project.projectFile))}...`,
    );
  }

  let slnLines = fs
    .readFileSync(slnFile)
    .toString()
    .split('\r\n');

  let contentsChanged = false;

  // Check for the project entry block

  const slnDir = path.dirname(slnFile);
  const relProjectFile = path.relative(slnDir, project.projectFile);

  const projectTypeGuid = projectTypeGuidsByLanguage[project.projectLang];

  const projectGuid = project.projectGuid.toUpperCase();

  const projectEntryBlock = [
    `Project("${projectTypeGuid}") = "${
      project.projectName
    }", "${relProjectFile}", "${projectGuid}"`,
    'EndProject',
  ];

  if (!linesContainsBlock(slnLines, projectEntryBlock)) {
    if (verbose) {
      console.log(chalk.yellow('Missing project entry block.'));
    }

    const globalIndex = slnLines.indexOf('Global');
    insertBlockIntoLines(slnLines, projectEntryBlock, globalIndex);
    contentsChanged = true;
  }

  // Check for the project configuration platforms

  const slnConfigs = getBlockContentsFromLines(
    slnLines,
    '\tGlobalSection(SolutionConfigurationPlatforms) = preSolution',
    '\tEndGlobalSection',
    false,
  ).map(line => line.match(/\s+([\w|]+)\s=/)[1]);

  let projectConfigLines = [];

  slnConfigs.forEach(slnConfig => {
    projectConfigLines.push(
      `\t\t${projectGuid}.${slnConfig}.ActiveCfg = ${slnConfig.replace(
        'x86',
        'Win32',
      )}`,
    );
    projectConfigLines.push(
      `\t\t${projectGuid}.${slnConfig}.Build.0 = ${slnConfig.replace(
        'x86',
        'Win32',
      )}`,
    );
  });

  const projectConfigStartIndex = slnLines.indexOf(
    '\tGlobalSection(ProjectConfigurationPlatforms) = postSolution',
  );

  projectConfigLines.forEach(projectConfigLine => {
    if (slnLines.indexOf(projectConfigLine) < 0) {
      if (verbose) {
        console.log(chalk.yellow('Missing project config block.'));
      }

      const projectConfigEndIndex = slnLines.indexOf(
        '\tEndGlobalSection',
        projectConfigStartIndex,
      );

      slnLines.splice(projectConfigEndIndex, 0, projectConfigLine);
      contentsChanged = true;
    }
  });

  // Write out new solution file if there were changes
  if (contentsChanged) {
    if (verbose) {
      console.log(
        chalk.yellow(
          `${chalk.bold(path.basename(slnFile))} needs to be updated.`,
        ),
      );
    }

    if (!checkMode) {
      if (verbose) {
        console.log(
          `Writing changes to ${chalk.bold(path.basename(slnFile))}...`,
        );
      }

      const slnContents = slnLines.join('\r\n');
      fs.writeFileSync(slnFile, slnContents, {
        encoding: 'utf8',
        flag: 'w',
      });
    }
  } else {
    if (verbose) {
      console.log(`No changes to ${chalk.bold(path.basename(slnFile))}.`);
    }
  }

  return contentsChanged;
}

module.exports = {
  addProjectToSolution: addProjectToSolution,
};
