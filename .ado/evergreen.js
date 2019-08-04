// @ts-check
// Script to keep sdx-platform up to date with GitHub/Microsoft/react-native

const path = require('path');
const request = require('request');
const execSync = require('child_process').execSync;
const fs = require('fs');
const branchNamePrefix = 'auto-update-versions';
const finalTargetBranchName = 'master';
let branchName;
let rnVersion;
let listOfChanges;

function exec(command) {
  try {
    console.log(`Running command: ${command}`);
    return execSync(command, {
      stdio: 'inherit'
    });
  } catch (err) {
    process.exitCode = 1;
    console.log(`Failure running: ${command}`);
    throw err;
  }
}

function sanitizeBranchName(branch) {
  return branch.replace('.', '').replace('-', '');
}

let myArgs = process.argv.slice(2);
let autopr = false;
let gitHubToken = null;

while (myArgs.length) {
  switch (myArgs[0]) {
    case 'autopr':
      autopr = true;
      // This args should be the result of new Buffer(":" + <githubToken>).toString("base64")
      gitHubToken = myArgs[1];
      myArgs = myArgs.slice(2);
      break;
    default:
      throw new Error('Invalid args');
  }
}

// Create a new VSO PR
function createPr() {
  console.log('Creating PR...');
  const url = 'https://api.github.com/repos/microsoft/react-native-windows/pulls';
  request.post(
    {
      url: url,
      json: true,
      headers: {
        Authorization: "Basic " + gitHubToken,
        "Content-Type": 'application/json',
        "User-Agent": "RNW-Evergreen Script"
      },
      body: {
        head: branchName,
        base: finalTargetBranchName,
        title: `Update to react-native@${rnVersion}`,
        body: `Automatic update to latest version published from @Microsoft/react-native, includes these changes:\n\`\`\`\n${listOfChanges}\n\`\`\``
      }
    },
    function (err, httpResponse, body) {
      console.log('HTTP Response \r\n -------------------------------------------------------------------------', httpResponse);
      //console.log("Body Response:", body);
      //console.log("err: " + err + "  body.errorCode" + body.errorCode);
      if (err || body.errorCode) {
        throw new Error(err || body.errorCode);
      }

      if (!body.number) {
        throw new Error('Failed to create PR.\nBody : ' + JSON.stringify(body));
      }

      const prId = body.number;

      // Trigger automation
      request.post(
        {
          url: `https://api.github.com/repos/microsoft/react-native-windows/issues/${prId}/comments`,
          json: true,
          headers: {
            Authorization: "Basic " + gitHubToken,
            "Content-Type": 'application/json',
            "User-Agent": "RNW-Evergreen Script"
          },
          body: {
            body: "/azp run"
          }
        }
        , function (err, httpResponse, body) {
          console.log('HTTP Response \r\n -------------------------------------------------------------------------', httpResponse);

          // Trigger AutoMerge
          request.post(
            {
              url: `https://api.github.com/repos/microsoft/react-native-windows/issues/${prId}/labels`,
              json: true,
              headers: {
                Authorization: "Basic " + gitHubToken,
                "Content-Type": 'application/json',
                "User-Agent": "RNW-Evergreen Script"
              },
              body: {
                labels: ["AutoMerge"]
              }
            }, function (err, httpResponse, body) {
              console.log('HTTP Response \r\n -------------------------------------------------------------------------', httpResponse);
            });
        });
    });
}

request.get('https://raw.githubusercontent.com/microsoft/react-native/master/package.json', function (error, response, body) {
  console.log(`Get react-native's package.json request result: `);
  console.log('error:', error); // Print the error if one occurred
  console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
  console.log('body:', body);

  if (error) {
    console.error(`Failed to get react-native's package.json`);
    throw error;
  }
  if (response.statusCode !== 200) {
    console.error(`Failed to get react-native's package.json`);
    throw new Error(`Failed to get react-native's package.json`);
  }

  const pkgJson = JSON.parse(body);
  const pkgJsonPath = path.resolve(__dirname, '../vnext/package.json');
  rnVersion = pkgJson.version;

  let existingPkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, 'utf8'));
  const existingRnVersion = existingPkgJson.devDependencies['react-native'].slice(existingPkgJson.devDependencies['react-native'].indexOf('archive/v') + 9, existingPkgJson.devDependencies['react-native'].indexOf('.tar.gz'));

  if (existingRnVersion === pkgJson.version) {
    console.log(`Already at latest react-native version: ${pkgJson.version}.`);
    console.log('Nothing to do...exiting');
    process.exitCode = 0;
    return;
  }

  // Collect log of changes included in this sync
  // Fetch older version so that we can get the log from there to here..
  try {
    exec(`git remote add msrn https://github.com/microsoft/react-native.git`);
  } catch (e) {
  }
  exec(`git fetch msrn`);
  listOfChanges = execSync(`git log --pretty=oneline --abbrev-commit v${existingRnVersion}..v${pkgJson.version}`).toString();


  branchName = branchNamePrefix + sanitizeBranchName(pkgJson.version);

  exec(`git checkout -b ${branchName} --track origin/${finalTargetBranchName}`);
  exec(`git pull`);

  console.log(`Updating react-native to version: ${pkgJson.version}`);
  const lernaLocation = path.resolve(__dirname, '../node_modules/lerna/cli.js');
  const setRNVersion = path.resolve(__dirname, 'setRNVersion.js');
  const repoRoot = path.resolve(__dirname, '..');
  execSync(`node ${lernaLocation} exec node ${setRNVersion} ${pkgJson.version} ${repoRoot}`);

  process.chdir(path.resolve(__dirname, '..'));

  // Run yarn install to update yarn.lock
  exec(`yarn install`);

  exec(`git add ${path.resolve(__dirname, '../yarn.lock')}`);
  exec(`git add ${path.resolve(__dirname, '..')}`);
  exec(`git commit -m "Update to react-native@${pkgJson.version}"`);
  exec(`git push origin ${branchName}`);

  if (autopr) createPr();
});
