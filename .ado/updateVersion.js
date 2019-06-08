// @ts-check
// Used to update the package version

const path = require("path");
const execSync = require("child_process").execSync;
const {
  pkgJsonPath,
  publishBranchName,
  updateVersionsInFiles,
  win32VersionRcPath
} = require("./versionUtils");

function exec(command) {
  try {
    console.log(`Running command: ${command}`);
    return execSync(command, {
      stdio: "inherit"
    });
  } catch (err) {
    process.exitCode = 1;
    console.log(`Failure running: ${command}`);
    throw err;
  }
}

function updateVersion() {
  // Set env variable to allow npm publish task to publish to correct tag
  console.log(
    `##vso[task.setvariable variable=npmTag]${
      publishBranchName === "master" ? "vnext" : publishBranchName
    }`
  );
  console.log(`Target branch to publish to: ${publishBranchName}`);

  const tempPublishBranch = `auto-update-version-publish-temp-${Date.now()}`;

  console.log(
    `Using ${`(.*-microsoft)(-${publishBranchName})?\\.([0-9]*)`} to match version`
  );

  exec(`npm install -g yarn`);

  // Ensure publishBranchName is same locally as remote
  exec(`git fetch origin ${publishBranchName}`);
  exec(`git checkout ${publishBranchName} --force`);
  exec(`git reset --hard origin/${publishBranchName}`);

  exec(`git checkout -b ${tempPublishBranch}`);

  const releaseVersion = updateVersionsInFiles();

  process.chdir(path.resolve(__dirname, "../vnext"));
  exec(
    `${process.env.APPDATA}\\npm\\node_modules\\yarn\\bin\\yarn.cmd install`
  );
  exec(`${process.env.APPDATA}\\npm\\node_modules\\yarn\\bin\\yarn.cmd build`);

  const tagName = "vnext-" + releaseVersion;

  exec(`git add ${pkgJsonPath}`);
  exec(`git add ${win32VersionRcPath}`);

  exec(`git commit -m "Applying package update to ${releaseVersion}`);
  exec(`git tag ${tagName}`);
  exec(`git push origin HEAD:${tempPublishBranch} --follow-tags --verbose`);
  exec(`git push origin tag ${tagName}`);

  exec(`git checkout ${publishBranchName}`);
  exec(`git pull origin ${publishBranchName}`);
  exec(`git merge ${tempPublishBranch} --no-edit`);
  exec(`git push origin HEAD:${publishBranchName} --follow-tags --verbose`);
  exec(`git branch -d ${tempPublishBranch}`);
  exec(`git push origin --delete -d ${tempPublishBranch}`);
}

updateVersion();
