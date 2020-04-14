const Launcher = require('@wdio/cli').default;
const path = require('path');

const specFolder = 'wdio/test';
let opts = process.argv
  .splice(2)
  .map(spec => path.join(specFolder, spec + '.spec.ts'));
console.log(`argv = ${opts}`);
const wdio = new Launcher('wdio.conf.js', { specs: opts });

// } process.argv || '**/**/**.ts');
wdio.run().then(
  code => {
    process.exit(code);
  },
  error => {
    console.error('Launcher failed to start the test', error.stacktrace);
    process.exit(1);
  }
);
