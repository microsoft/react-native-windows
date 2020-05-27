const path = require('path');
const fs = require('fs');
const xml2js = require('xml2js');
const parser = new xml2js.Parser({ attrkey: 'ATTR' });
const child_process = require('child_process');
const prompt = require('prompt-sync')();
const chalk = require('chalk');

const specFolder = 'wdio/test';

function getMetadata(specPath) {
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
function filterSpec(specPath) {
  const metadata = getMetadata(specPath);
  for (const metadataElement of metadata) {
    if (filters[metadataElement](specPath)) {
      return false;
    }
  }
  return true;
}

function selectSpecs(folder) {
  let specs = [];
  if (process.argv.length > 2) {
    specs = process.argv.splice(2).map(spec => spec + '.spec.ts');
  } else {
    specs = fs.readdirSync(folder).filter(x => x.endsWith('.spec.ts'));
  }
  specs = specs.map(spec => path.join(folder, spec)).filter(filterSpec);
  return specs;
}

let opts = selectSpecs(specFolder);
console.log(`Selected tests: ${opts}`);

function ensureRunningInHyperV() {
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
      console.error(`Something went wrong processing file ${logfile}`);
    } else {
      for (const testsuite of res.testsuites.testsuite) {
        const attr = testsuite.ATTR;

        if (attr.errors > 0 || attr.failures > 0) {
          const name = attr.name;
          failures[name] = {};

          for (const testcase of testsuite.testcase) {
            if (testcase.error && testcase.error[0].ATTR) {
              failures[name].testcase = testcase.ATTR.name;
              failures[name].error = testcase.error[0].ATTR.message;
              const systemErr = testcase['system-err'][0];
              const stack = systemErr.substr(
                systemErr.indexOf('\n    at ') + 1
              );
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
    .filter(x => x != null && x != '');
  return names;
}

function printFailedTests(ft) {
  console.log('Failed test cases: ');
  for (const key in ft) {
    console.log(chalk.redBright(key));
    console.log(
      '  ',
      chalk.underline('testcase'),
      chalk.bold(ft[key].testcase)
    );
    console.log('  ', chalk.underline('error'), chalk.bold(ft[key].error));
    console.log('  ', chalk.underline('stack'));
    console.log(ft[key].stack);
  }
}

function doProcess(code) {
  const failedTests = parseLogs();
  for (const failedTest of failedTests) {
    if (Object.keys(failedTest).length > 0) {
      printFailedTests(failedTest);
    }
  }
  process.exit(code);
}

function runWdio() {
  ensureRunningInHyperV();

  wdio.run().then(
    code => {
      try {
        doProcess(code);
      } catch (e) {
        console.log(e);
        // any exception that isn't handled is an error
        process.exit(3);
      }
    },
    error => {
      console.error('Launcher failed to start the test', error.stacktrace);
      process.exit(1);
    }
  );
}

try {
  runWdio();
} catch (e) {
  console.log(e);
  // any exception that isn't handled is an error
  process.exit(2);
}
