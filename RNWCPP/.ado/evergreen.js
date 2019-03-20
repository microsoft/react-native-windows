// @ts-check

// Script to keep sdx-platform up to date with GitHub/Microsoft/react-native

const path = require('path');
const request = require('request');
const execSync = require('child_process').execSync;
const fs = require('fs');
const branchNamePrefix = 'auto-update-versions';
const finalTargetBranchName = 'rnwcpp-preview';
let branchName;
let rnVersion;

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
  const url = 'https://api.github.com/repos/Microsoft/react-native-windows/pulls';
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
        title: `Update to react-native@${rnVersion}`
      }
    },
    function(err, httpResponse, body) {
      console.log('HTTP Response \r\n -------------------------------------------------------------------------', httpResponse);
      //console.log("Body Response:", body);
      //console.log("err: " + err + "  body.errorCode" + body.errorCode);
      if (err || body.errorCode) {
        throw new Error(err || body.errorCode);
      }

      if (!body.pullRequestId) {
        throw new Error('Failed to create PR.\nBody : ' + JSON.stringify(body));
      }
    }
  );
}

request.get('https://raw.githubusercontent.com/Microsoft/react-native/master/package.json', function(error, response, body) {
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
  const pkgJsonPath = path.resolve(__dirname, '../package.json');

  let existingPkgJson = JSON.parse(fs.readFileSync(pkgJsonPath, 'utf8'));

  if (existingPkgJson.dependencies['react-native'] === pkgJson.version) {
    console.log(`Already at latest react-native version: ${pkgJson.version}.`);
    console.log('Nothing to do...exiting');
    process.exitCode = 0;
    return;
  }

  rnVersion = pkgJson.version;

  console.log(`Updating react-native to version: ${rnVersion}`);
  existingPkgJson.dependencies['react-native'] = rnVersion;

  branchName = branchNamePrefix + sanitizeBranchName(rnVersion);

  exec(`git checkout ${finalTargetBranchName}`);
  exec(`git checkout -b ${branchName}`);
  fs.writeFileSync(pkgJsonPath, JSON.stringify(existingPkgJson, null, 2));
  exec(`git add ${pkgJsonPath}`);
  exec(`git commit -m "Update to react-native@${rnVersion}"`);
  exec(`git push origin ${branchName}`);

  if (autopr) createPr();
});
