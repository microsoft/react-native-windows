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

// Set env variable to allow VS to build dll with correct version information
console.log(
  `##vso[task.setvariable variable=RNW_PKG_VERSION_STR]${pkgJson.version}`
);
// Set env variable to allow VS to build dll with correct version information
console.log(
  `##vso[task.setvariable variable=RNW_PKG_VERSION]${npmVersionToRcVersion(pkgJson.version)}`
);

console.log(`##vso[task.setvariable variable=npmVersion;isOutput=true]${pkgJson.version}`);

// Record commit number, so that additional build tasks can sync to changes
const commitId = child_process.execSync(`git rev-list HEAD -n 1`).toString();
console.log(`##vso[task.setvariable variable=publishCommitId;isOutput=true]${commitId}`);