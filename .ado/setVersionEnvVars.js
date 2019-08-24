// @ts-check
const fs = require("fs");
const path = require("path");
const child_process = require('child_process');

const pkgJsonPath = path.resolve(__dirname, "../vnext/package.json");

// Helper to format npmVersion in a way that the Version.rc resource files want it
function npmVersionToRcVersion(npmVersion) {
  let groups = npmVersion.split(/[\.-]/);
  return `${groups[0]},${groups[1]},${groups[2]},${groups[4]}`;
}

let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, "utf8"));

// Record commit number, so that additional build tasks can record that commit in the nuget
const commitId = child_process.execSync(`git rev-list HEAD -n 1`).toString().trim();

let versionEnvVars = {
  RNW_PKG_VERSION_STR: pkgJson.version,
  RNW_PKG_VERSION: npmVersionToRcVersion(pkgJson.version),
  npmVersion: pkgJson.version,
  publishCommitId: commitId
}

// Set the vars using the vso format for now too - while migrating from the vso publish task to the github actions one.

// Set env variable to allow VS to build dll with correct version information
console.log(`##vso[task.setvariable variable=RNW_PKG_VERSION_STR]${versionEnvVars.RNW_PKG_VERSION_STR}`);
// Set env variable to allow VS to build dll with correct version information
console.log(`##vso[task.setvariable variable=RNW_PKG_VERSION]${versionEnvVars.RNW_PKG_VERSION}`);
console.log(`##vso[task.setvariable variable=npmVersion;isOutput=true]${versionEnvVars.npmVersion}`);
console.log(`##vso[task.setvariable variable=publishCommitId;isOutput=true]${versionEnvVars.publishCommitId}`);


const dirPath = path.resolve(require('os').tmpdir(), 'versionEnvVars');
if (!fs.existsSync(dirPath)) {
fs.mkdirSync(dirPath);
}

// I set the env var using both :: and ##, since github is in the process of changing the format
fs.writeFileSync(path.resolve(dirPath, 'versionEnvVars.js'), 
`
console.log("::[set-env name=RNW_PKG_VERSION_STR;]${versionEnvVars.RNW_PKG_VERSION_STR}");
console.log("##[set-env name=RNW_PKG_VERSION_STR;]${versionEnvVars.RNW_PKG_VERSION_STR}");
console.log("::[set-env name=RNW_PKG_VERSION;]${versionEnvVars.RNW_PKG_VERSION}");
console.log("##[set-env name=RNW_PKG_VERSION;]${versionEnvVars.RNW_PKG_VERSION}");
console.log("::[set-env name=npmVersion;]${versionEnvVars.npmVersion}");
console.log("##[set-env name=npmVersion;]${versionEnvVars.npmVersion}");
console.log("::[set-env name=publishCommitId;]${versionEnvVars.publishCommitId}");
console.log("##[set-env name=publishCommitId;]${versionEnvVars.publishCommitId}");
`);
fs.writeFileSync(path.resolve(dirPath, 'versionEnvVars.json'), JSON.stringify(versionEnvVars));

