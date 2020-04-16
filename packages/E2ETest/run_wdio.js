const path = require('path');
const fs = require('fs');
const xml2js = require('xml2js');
const parser = new xml2js.Parser({ attrkey: 'ATTR' });
const child_process = require('child_process');
const prompt = require('prompt-sync')();

const specFolder = 'wdio/test';

function GetMetadata(specPath) {
  const contents = fs.readFileSync(specPath);
  const metadataTag = '// @metadata ';
  const metadataStart = contents.indexOf(metadataTag);
  if (metadataStart != -1) {
    let metadata = contents
      .toString()
      .substr(metadataStart + metadataTag.length)
      .split(/[\r\n]/)[0];
    return metadata.split(' ');
  }
  return [];
}

const filters = {
  SkipCI: specPath => {
    return process.env.BUILD_QUEUEDBY == 'GitHub';
  },
};

// Returns true if the spec is to run.
// Specs marked SkipCI are excluded from CI (identified by environment variables in the ADO lab)
function FilterSpec(specPath) {
  const metadata = GetMetadata(specPath);
  for (let i = 0; i < metadata.length; i++) {
    if (filters[metadata[i]](specPath)) {
      return false;
    }
  }
  return true;
}

function SelectSpecs(folder) {
  let specs = [];
  if (process.argv.length > 2) {
    specs = process.argv.splice(2).map(spec => spec + '.spec.ts');
  } else {
    specs = fs.readdirSync(folder).filter(x => x.endsWith('.spec.ts'));
  }
  specs = specs.map(spec => path.join(folder, spec)).filter(FilterSpec);
  return specs;
}

let opts = SelectSpecs(specFolder);
console.log(`Selected tests: ${opts}`);


function OverrideHyperV() {
  const isHyperV_VM = child_process.execSync('powershell.exe (gwmi Win32_BaseBoard).Manufacturer -eq \\\"Microsoft Corporation\\\"');
  if (isHyperV_VM != 'True') {
    console.log('Not running in Hyperv');
    const answer = prompt('E2ETest is meant to be run in a HyperV VM. Continue? (Y/N)');
    if (answer.toUpperCase() != 'Y') { process.exit(0); }
  }
}

OverrideHyperV();

const Launcher = require('@wdio/cli').default;

const wdio = new Launcher('wdio.conf.js', { specs: opts });

function parseLog(logfile) {
  const xmlString = fs.readFileSync(logfile);
  let name;
  parser.parseString(xmlString, (err, res) => {
    if (!res.testsuites) {
      name = 'something went wrong';
    } else {
      const attr = res.testsuites.testsuite[0].ATTR;
      if (attr.errors > 0 || attr.failures > 0) {
        name = attr.name;
      }
    }
  });
  return name;
}

function parseLogs() {
  const reportsDir = path.join(__dirname, 'reports');
  const logs = fs.readdirSync(reportsDir).filter(x => x.endsWith('.log'));
  const names = logs
    .map(x => parseLog(path.join(reportsDir, x)))
    .filter(x => x != null);
  return names;
}

function Process(code) {
  const failedTests = parseLogs();
  for (let i = 0; i < failedTests.length; i++) {
    console.log(`Failed test: ${failedTests[i]}`);
  }
  process.exit(code);
}

process.exit(4);
wdio.run().then(
  code => {
    Process(code);
  },
  error => {
    console.error('Launcher failed to start the test', error.stacktrace);
    process.exit(1);
  }
);
