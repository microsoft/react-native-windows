// Used to publish react-native-win

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

function doPublish() {
  const publishBranchName = process.env.publishBranchName;

  const tempPublishBranch = `publish-${Date.now()}`;

  const pkgJsonPath = path.resolve(__dirname, "../package.json");
  let pkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, "utf8"));

  let releaseVersion = pkgJson.version;

  const versionGroups = /(.*-v\.)([0-9]*)/.exec(releaseVersion);
  if (versionGroups) {
    releaseVersion = versionGroups[1] + (parseInt(versionGroups[2]) + 1);
  } else {
    if (releaseVersion.indexOf("-") === -1) {
      releaseVersion = releaseVersion + "-v.0";
    } else {
      console.log("Invalid version to publish");
      exit(1);
    }
  }

  pkgJson.version = releaseVersion;
  fs.writeFileSync(pkgJsonPath, JSON.stringify(pkgJson, null, 2));
  console.log(`Updating package.json to version ${releaseVersion}`);

  exec(`git checkout -b ${tempPublishBranch}`);

  exec(`git add ${pkgJsonPath}`);
  exec(`git commit -m "Applying package update to ${releaseVersion}`);
  exec(`git tag v${releaseVersion}`);
  exec(`git push origin HEAD:${tempPublishBranch} --follow-tags --verbose`);
  exec(`git push origin tag v${releaseVersion}`);

  exec(`npm publish`);

  exec(`git checkout ${publishBranchName}`);
  exec(`git pull origin ${publishBranchName}`);
  exec(`git merge ${tempPublishBranch} --no-edit`);
  exec(
    `git push origin HEAD:${publishBranchName} --follow-tags --verbose`
  );
  exec(`git branch -d ${tempPublishBranch}`);
  exec(`git push origin --delete -d ${tempPublishBranch}`);
}

doPublish();