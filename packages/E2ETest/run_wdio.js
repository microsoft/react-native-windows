const Launcher = require('@wdio/cli').default;
const path = require('path');
const fs = require('fs');
const xml2js = require('xml2js');
const parser = new xml2js.Parser({ attrkey: 'ATTR' });

const specFolder = 'wdio/test';
let opts = process.argv
  .splice(2)
  .map(spec => path.join(specFolder, spec + '.spec.ts'));

const wdio = new Launcher('wdio.conf.js', { specs: opts });

function parseLog(logfile) {
  const xmlString = fs.readFileSync(logfile);
  let name;
  parser.parseString(xmlString, (err, res) => {
    name = res.testsuites.testsuite[0].ATTR.name;
  });
  return name;
}

function parseLogs() {
  const reportsDir = path.join(__dirname, 'reports');
  const logs = fs.readdirSync(reportsDir).filter(x => x.endsWith('.log'));
  const names = logs.map(x => parseLog(path.join(reportsDir, x)));
  return names;
}

function Process(code) {
  const failedTests = parseLogs();
  for (let i = 0; i < failedTests.length; i++) {
    console.log(`Failed test: ${failedTests[i]}`);
  }
  process.exit(code);
}

wdio.run().then(
  code => {
    Process(code);
  },
  error => {
    console.error('Launcher failed to start the test', error.stacktrace);
    process.exit(1);
  }
);
