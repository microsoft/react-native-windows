const fs = require('fs');
const path = require('path');
const os = require('os');
const https = require('https');
const child_process = require('child_process');

const REQUIRED_VERSION = {
    simple: 'v4.0.0',
    reportedByCommand: '4.0.0.2283'
}

main();

function main() {
  const nugetDir = path.join(
    process.cwd(),
    'local-cli',
    '.nuget'
  );
  
  if (!fs.existsSync(nugetDir)) {
    fs.mkdirSync(nugetDir);
  }
  
  const nugetPath = path.join(nugetDir, 'nuget.exe');

  if (checkNugetVersion(nugetPath, REQUIRED_VERSION.reportedByCommand)) {
    console.info('Download NuGet.exe: skipped because there is already one with correct version');
  } else {
    downloadNuget(nugetPath, REQUIRED_VERSION.simple);
    console.info('Download NuGet.exe: done')
  }
}

function checkNugetVersion(path, expectedVersion) {
  if (!fs.existsSync(path)) {
    return false;
  }

  try {
    const output = child_process.execFileSync(path, [], { encoding: 'utf-8' });

    const actualVersion = /^NuGet Version: (\S*)$/m.exec(output)[1];
    return actualVersion == expectedVersion;
  } catch (e) {
    return false;
  }
}

function downloadNuget(path, version) {
  fileStream = fs.createWriteStream(path);
  const url = `https://dist.nuget.org/win-x86-commandline/${version}/nuget.exe`;
  https.get(url, (res) => {
    res.pipe(fileStream);
  });
}
