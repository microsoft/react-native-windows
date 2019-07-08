// @ts-check
const fs = require("fs");
const path = require("path");

const publishBranchName = process.env.BUILD_SOURCEBRANCH.match(
  /refs\/heads\/(.*)/
)[1];

const pkgJsonPath = path.resolve(__dirname, "../vnext/package.json");

const win32VersionRcPath = path.resolve(
  __dirname,
  "../vnext/Desktop.DLL/Version.rc"
);
const uwpVersionRcPath = path.resolve(
  __dirname,
  "../vnext/ReactUWP/Version.rc"
);

function updateRcVersion(releaseVersion, versionGroups, rcPath)
{
  let versionRC = fs.readFileSync(rcPath).toString();
  versionRC = versionRC.replace(
    /#define VER_FILEVERSION_STR\s+"[^"]+"/,
    `#define VER_FILEVERSION_STR "${releaseVersion}"`
  );
  versionRC = versionRC.replace(
    /#define VER_FILEVERSION\s+.+/,
    `#define VER_FILEVERSION ${releaseVersion.split(".")[0]},${
      releaseVersion.split(".")[1]
    },${releaseVersion.split(".")[2].split("-")[0]},${parseInt(
      versionGroups[3]
    ) + 1}`
  );
  fs.writeFileSync(rcPath, versionRC);
}

function updateVersionsInFiles() {
  const branchVersionSuffix = publishBranchName.match(/(fb.*merge)|(fabric)/)
    ? `-${publishBranchName}`
    : "";

  let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, "utf8"));

  let releaseVersion = pkgJson.version;

  const versionStringRegEx = new RegExp(
    `(.*-vnext)(-${publishBranchName})?\\.([0-9]*)`
  );
  const versionGroups = versionStringRegEx.exec(releaseVersion);
  if (versionGroups) {
    releaseVersion =
      versionGroups[1] +
      branchVersionSuffix +
      "." +
      (parseInt(versionGroups[3]) + 1);
  } else {
    if (releaseVersion.indexOf("-") === -1) {
      releaseVersion = releaseVersion + `-vnext${branchVersionSuffix}.0`;
    } else {
      console.log("Invalid version to publish");
      process.exit(1);
    }
  }

  pkgJson.version = releaseVersion;
  fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));
  console.log(`Updating package.json to version ${releaseVersion}`);

  updateRcVersion(releaseVersion, versionGroups, win32VersionRcPath);
  updateRcVersion(releaseVersion, versionGroups, uwpVersionRcPath);

  return releaseVersion;
}

module.exports = {
  pkgJsonPath,
  publishBranchName,
  updateVersionsInFiles,
  win32VersionRcPath,
  uwpVersionRcPath
};
