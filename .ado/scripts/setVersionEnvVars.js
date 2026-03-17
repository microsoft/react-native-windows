// @ts-check
const fs = require('fs');
const path = require('path');
const process = require('process');
const child_process = require('child_process');

const pkgJsonPath = path.resolve(__dirname, "../../vnext/package.json");
const pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, "utf8"));

// Record commit number, so that additional build tasks can record that commit in the NuGet
const commitId = child_process.execSync(`git rev-list HEAD -n 1`).toString().trim();

const buildEnvironment = process.argv[2];
let isPr = buildEnvironment.endsWith("PullRequest");
let isCi = buildEnvironment === "Continuous"
if (!isPr && !isCi) {
  throw new Error("Must pass argument indicating if this is for a 'PullRequest' or 'Continuous");
}
const buildId = process.argv[3];
if (buildId == undefined) {
  throw new Error("Missing argument for the buildid")
}

let adoBuildVersion=pkgJson.version;
if (isPr) {
  adoBuildVersion += `.pr${buildId}`;
}

const versionSegments = pkgJson.version.split('.');

const versionEnvVars = {
  RNW_PKG_VERSION_STR: pkgJson.version,
  RNW_PKG_VERSION_MAJOR: versionSegments[0],
  RNW_PKG_VERSION_MINOR: versionSegments[1],
  RNW_PKG_VERSION_PATCH: versionSegments[2],
  npmVersion: pkgJson.version,
  publishCommitId: commitId,
  reactDevDependency: pkgJson.devDependencies['react'],
  reactNativeDevDependency: pkgJson.devDependencies['react-native'],
  npmDistTag: pkgJson?.beachball?.defaultNpmTag?.trim(),
}

if (!versionEnvVars.npmDistTag) {
  throw new Error('defaultNpmTag is missing in vnext/package.json');
}

// Set the build number so the build in the publish pipeline and the release pipeline are named with the convenient version
// See: https://docs.microsoft.com/en-us/azure/devops/pipelines/scripts/logging-commands?view=azure-devops&tabs=bash#updatebuildnumber-override-the-automatically-generated-build-number
console.log(`##vso[build.updatebuildnumber]RNW_${adoBuildVersion}`)

// Set env variable to allow VS to build dll with correct version information
console.log(`##vso[task.setvariable variable=RNW_PKG_VERSION_STR]${versionEnvVars.RNW_PKG_VERSION_STR}`);
console.log(`##vso[task.setvariable variable=RNW_PKG_VERSION_MAJOR]${versionEnvVars.RNW_PKG_VERSION_MAJOR}`);
console.log(`##vso[task.setvariable variable=RNW_PKG_VERSION_MINOR]${versionEnvVars.RNW_PKG_VERSION_MINOR}`);
console.log(`##vso[task.setvariable variable=RNW_PKG_VERSION_PATCH]${versionEnvVars.RNW_PKG_VERSION_PATCH}`);

// Set other version related variables used by CI
console.log(`##vso[task.setvariable variable=npmVersion]${versionEnvVars.npmVersion}`);
console.log(`##vso[task.setvariable variable=publishCommitId]${versionEnvVars.publishCommitId}`);
console.log(`##vso[task.setvariable variable=reactDevDependency]${versionEnvVars.reactDevDependency}`);
console.log(`##vso[task.setvariable variable=reactNativeDevDependency]${versionEnvVars.reactNativeDevDependency}`);
console.log(`##vso[task.setvariable variable=NpmDistTag]${versionEnvVars.npmDistTag}`);

const runnerTemp = process.env.RUNNER_TEMP;
if (!runnerTemp) {
  throw new Error('RUNNER_TEMP environment variable is not set');
}

const dirPath = path.resolve(runnerTemp, 'versionEnvVars');
fs.mkdirSync(dirPath, {recursive: true});

fs.writeFileSync(path.resolve(dirPath, 'versionEnvVars.js'),
`
console.log("##vso[task.setvariable variable=RNW_PKG_VERSION_STR]${versionEnvVars.RNW_PKG_VERSION_STR}");
console.log("##vso[task.setvariable variable=RNW_PKG_VERSION_MAJOR]${versionEnvVars.RNW_PKG_VERSION_MAJOR}");
console.log("##vso[task.setvariable variable=RNW_PKG_VERSION_MINOR]${versionEnvVars.RNW_PKG_VERSION_MINOR}");
console.log("##vso[task.setvariable variable=RNW_PKG_VERSION_PATCH]${versionEnvVars.RNW_PKG_VERSION_PATCH}");
console.log("##vso[task.setvariable variable=npmVersion]${versionEnvVars.npmVersion}");
console.log("##vso[task.setvariable variable=publishCommitId]${versionEnvVars.publishCommitId}");
console.log("##vso[task.setvariable variable=reactDevDependency]${versionEnvVars.reactDevDependency}");
console.log("##vso[task.setvariable variable=reactNativeDevDependency]${versionEnvVars.reactNativeDevDependency}");
console.log("##vso[task.setvariable variable=NpmDistTag]${versionEnvVars.npmDistTag}");
`);
