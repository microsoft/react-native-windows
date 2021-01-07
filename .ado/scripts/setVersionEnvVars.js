// @ts-check
const fs = require('fs'); 
const path = require('path');
const child_process = require('child_process');
const semver = require('semver');

const pkgJsonPath = path.resolve(__dirname, "../../vnext/package.json");
const pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, "utf8"));

// Record commit number, so that additional build tasks can record that commit in the NuGet
const commitId = child_process.execSync(`git rev-list HEAD -n 1`).toString().trim();

const versionEnvVars = {
  RNW_PKG_VERSION_STR: pkgJson.version,
  RNW_PKG_VERSION_MAJOR: semver.major(pkgJson.version),
  RNW_PKG_VERSION_MINOR: semver.minor(pkgJson.version),
  RNW_PKG_VERSION_PATCH: semver.patch(pkgJson.version),
  npmVersion: pkgJson.version,
  publishCommitId: commitId,
  reactDevDependency: pkgJson.devDependencies['react'],
  reactNativeDevDependency: pkgJson.devDependencies['react-native'],
}

// Set the build number so the build in the publish pipeline and the release pipeline are named with the convenient version
// See: https://docs.microsoft.com/en-us/azure/devops/pipelines/scripts/logging-commands?view=azure-devops&tabs=bash#updatebuildnumber-override-the-automatically-generated-build-number
console.log(`##vso[build.updatebuildnumber]RNW_${versionEnvVars.npmVersion}`)

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

const dirPath = path.resolve(process.env.RUNNER_TEMP, 'versionEnvVars');
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
`);
