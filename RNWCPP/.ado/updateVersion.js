// Used to update the package version 

const fs = require("fs");
const path = require("path");
const execSync = require("child_process").execSync;

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
  const publishBranchName = process.env.publishBranchName;
  const tempPublishBranch = `publish-${Date.now()}`;

  exec(`npm install -g yarn`);

  // Ensure publishBranchName is same locally as remote
  exec(`git fetch origin ${publishBranchName}`);
  exec(`git checkout ${publishBranchName} --force`);
  exec(`git reset --hard origin/${publishBranchName}`);
  
  exec(`git checkout -b ${tempPublishBranch}`);

  const pkgJsonPath = path.resolve(__dirname, "../package.json");
  let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, "utf8"));

  let releaseVersion = pkgJson.version;

  const versionGroups = /(.*-vnext\.)([0-9]*)/.exec(releaseVersion);
  if (versionGroups) {
    releaseVersion = versionGroups[1] + (parseInt(versionGroups[2]) + 1);
  } else {
    if (releaseVersion.indexOf("-") === -1) {
      releaseVersion = releaseVersion + "-vnext.0";
    } else {
      console.log("Invalid version to publish");
      exit(1);
    }
  }

  pkgJson.version = releaseVersion;
  fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));
  console.log(`Updating package.json to version ${releaseVersion}`);

  process.chdir(path.resolve(__dirname, ".."));
  exec(`${process.env.APPDATA}\\npm\\node_modules\\yarn\\bin\\yarn.cmd install`);
  exec(`${process.env.APPDATA}\\npm\\node_modules\\yarn\\bin\\yarn.cmd build`);

  const tagName = 'vnext-' + releaseVersion;

  exec(`git add ${pkgJsonPath}`);
  exec(`git commit -m "Applying package update to ${releaseVersion}`);
  exec(`git tag ${tagName}`);
  exec(`git push origin HEAD:${tempPublishBranch} --follow-tags --verbose`);
  exec(`git push origin tag ${tagName}`);

  exec(`git checkout ${publishBranchName}`);
  exec(`git pull origin ${publishBranchName}`);
  exec(`git merge ${tempPublishBranch} --no-edit`);
  exec(
    `git push origin HEAD:${publishBranchName} --follow-tags --verbose`
  );
  exec(`git branch -d ${tempPublishBranch}`);
  exec(`git push origin --delete -d ${tempPublishBranch}`);
}

updateVersion();