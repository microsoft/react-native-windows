/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

const {execFileSync} = require('node:child_process');

// Commit text and diagnostics are untrusted ADO log content,
// not logging commands.
const safeLog = text => String(text).replace(/##vso\[/gi, '##vso [');
const flag = value => (value ? 'True' : 'False');

function validateInputs(env, override) {
  if (!['auto', 'true', 'false'].includes(override)) {
    throw new Error('Expected one override argument: auto, true, or false');
  }
  const required = ['BUILD_REASON'];
  required.push(
    ...(env.BUILD_REASON === 'PullRequest'
      ? ['SYSTEM_PULLREQUEST_SOURCEBRANCH']
      : ['BUILD_SOURCEVERSION', 'BUILD_SOURCEBRANCH']),
  );
  for (const name of required) {
    if (!env[name]?.trim()) {
      throw new Error(`Missing required input: ${name}`);
    }
  }
}

// Pure classification: no Git, environment reads, or logging on this path.
function detectReleaseBuild(env, override, message = '') {
  validateInputs(env, override);
  let isReleaseCommit = false;
  let detected;
  let warning;
  let rule;
  if (env.BUILD_REASON === 'PullRequest') {
    detected =
      env.SYSTEM_PULLREQUEST_SOURCEBRANCH.startsWith('prepare-release/');
    rule = `PR source branch: ${env.SYSTEM_PULLREQUEST_SOURCEBRANCH}`;
  } else {
    const lines = message.split(/\r?\n/);
    // The marker is a body line; the subject never counts as one.
    const markers = lines
      .slice(1)
      .filter(line => line.startsWith('RNW-Release-Branch'));
    if (markers.length) {
      const branch = env.BUILD_SOURCEBRANCH.replace(/^refs\/heads\//, '');
      const match = /^RNW-Release-Branch: (\S+)$/.exec(markers[0]);
      rule = `Marker found: ${markers.join(' | ')}; target branch: ${branch}`;
      if (markers.length !== 1 || !match || match[1] !== branch) {
        const problem =
          'Invalid release marker (malformed, duplicate, or branch mismatch).' +
          ` ${rule}`;
        if (override === 'auto') {
          throw new Error(problem);
        }
        warning = problem;
      } else {
        isReleaseCommit = true;
      }
    } else {
      isReleaseCommit = lines[0].trim().startsWith('RELEASE:');
      rule = `Legacy subject rule used: ${lines[0]}`;
    }
    detected = isReleaseCommit;
  }
  return {
    isReleaseBuild: override === 'auto' ? detected : override === 'true',
    isReleaseCommit,
    warning,
    rule,
  };
}

function main() {
  try {
    const override = process.argv.length === 3 ? process.argv[2] : undefined;
    validateInputs(process.env, override);
    let message = '';
    if (process.env.BUILD_REASON !== 'PullRequest') {
      message = execFileSync(
        'git',
        ['show', '-s', '--format=%B', process.env.BUILD_SOURCEVERSION],
        {encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe']},
      );
      console.log(`Commit message:\n${safeLog(message)}`);
    }
    const result = detectReleaseBuild(process.env, override, message);
    if (result.warning) {
      console.log(
        `##vso[task.logissue type=warning]${safeLog(result.warning)}`,
      );
    }
    console.log(safeLog(`${result.rule}; override=${override}`));
    const build = flag(result.isReleaseBuild);
    const commit = flag(result.isReleaseCommit);
    console.log(`RESULT: isReleaseBuild=${build}; isReleaseCommit=${commit}`);
    console.log(
      `##vso[task.setvariable variable=isReleaseBuild;isOutput=true]${build}`,
    );
    console.log(
      `##vso[task.setvariable variable=isReleaseCommit;isOutput=true]${commit}`,
    );
    console.log(`##vso[task.setvariable variable=isReleaseBuild]${build}`);
  } catch (error) {
    console.error(`##vso[task.logissue type=error]${safeLog(error.message)}`);
    process.exitCode = 1;
  }
}

module.exports = {detectReleaseBuild, main};
if (require.main === module) {
  main();
}
