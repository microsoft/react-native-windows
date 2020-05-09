const path = require('path');
const fs = require('fs');
const xml2js = require('xml2js');
const parser = new xml2js.Parser({ attrkey: 'ATTR' });
const child_process = require('child_process');
const prompt = require('prompt-sync')();
const chalk = require('chalk');

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

function EnsureRunningInHyperV() {
  const baseboardMfr = child_process
    .execSync('powershell.exe (gwmi Win32_BaseBoard).Manufacturer')
    .toString()
    .replace(/[\r\n]/, '');
  if (!baseboardMfr.startsWith('Microsoft Corporation')) {
    console.log(`Not running in HyperV. Mfr = ${baseboardMfr}`);
    const answer = prompt(
      'E2ETest is meant to be run in a HyperV VM. Continue? (Y/N)'
    );
    if (answer.toUpperCase() != 'Y') {
      process.exit(0);
    }
  }
}

const Launcher = require('@wdio/cli').default;

const wdio = new Launcher('wdio.conf.js', { specs: opts });

function parseLog(logfile) {
  const xmlString = fs.readFileSync(logfile);
  let failures = {};
  parser.parseString(xmlString, (err, res) => {
    if (!res.testsuites) {
      failures = 'something went wrong';
    } else {
      for (let i = 0; i < res.testsuites.testsuite.length; i++) {
        const attr = res.testsuites.testsuite[i].ATTR;
        if (attr.errors > 0 || attr.failures > 0) {
          let name = attr.name;
          failures[name] = {};
          const testcases = res.testsuites.testsuite[i].testcase;
          for (let j = 0; j < testcases.length; j++) {
            if (testcases[j].error && testcases[j].error[0].ATTR) {
              failures[name].testcase = testcases[j].ATTR.name;
              failures[name].error = testcases[j].error[0].ATTR.message;
              const systemErr = testcases[j]['system-err'][0];
              const stack = systemErr.substr(systemErr.indexOf('\n    at ') + 1);
              failures[name].stack = stack;
            }
          }
        }

      }
    }
  });
  return failures;
}

function parseLogs() {
  const reportsDir = path.join(__dirname, 'reports');
  const logs = fs.readdirSync(reportsDir).filter(x => x.endsWith('.log'));
  const names = logs
    .map(x => parseLog(path.join(reportsDir, x)))
    .filter(x => x != null);
  return names;
}

function PrintFailedTests(ft) {
  for (let i = 0; i < Object.keys(ft).length; i++) {
    const key = Object.keys(ft)[i];
    console.log(chalk.redBright(key));
    console.log('  ', chalk.underline('testcase'), chalk.bold(ft[key].testcase));
    console.log('  ', chalk.underline('error'), chalk.bold(ft[key].error));
    console.log('  ', chalk.underline('stack'));
    console.log(ft[key].stack);
  }
}

function Process(code) {
  const failedTests = parseLogs();
  for (let i = 0; i < failedTests.length; i++) {
    console.log('Failed tests: ');
    PrintFailedTests(failedTests[i]);
  }
  process.exit(code);
}

function RunWdio() {
  EnsureRunningInHyperV();

  wdio.run().then(
    code => {
      Process(code);
    },
    error => {
      console.error('Launcher failed to start the test', error.stacktrace);
      process.exit(1);
    }
  );
}

RunWdio();
