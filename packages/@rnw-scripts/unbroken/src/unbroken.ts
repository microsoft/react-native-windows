/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Forked from https://github.com/asklar/unbroken
 * Original Copyright (c) 2020-2022 Alexander Sklar
 *
 * @format
 */

import {parseArgs} from 'node:util';
import * as path from 'path';

import fs from '@react-native-windows/fs';

import {Checker, Options} from './checker';

// ---------------------------------------------------------------------------
// CLI help
// ---------------------------------------------------------------------------

function showHelp(): void {
  const version = JSON.parse(
    fs.readFileSync(path.join(__dirname, '../package.json')).toString(),
  ).version;

  console.log(`
unbroken ${version} - no more broken links in markdown!

Usage:
  npx unbroken [directory] [options]

Options:
  -e, --exclusions <file>          The exclusions file (default: .unbroken_exclusions)
  -l, --local-only                 Do not test http and https links
  -a, --allow-local-line-sections  Allow line sections like foo.cpp#L12
  -i, --init                       Create a default exclusions file if one doesn't exist
  -q, --quiet                      Suppress output
  -s, --superquiet                 Maximum silence
      --parse-ids                  Allow anchors to point to Docusaurus id aliases
  -u, --user-agent <string>        Custom User-Agent string
      --help                       Show this help message

Project home: https://github.com/asklar/unbroken
`);
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

async function run() {
  const {values, positionals} = parseArgs({
    options: {
      exclusions: {type: 'string', short: 'e'},
      'local-only': {type: 'boolean', short: 'l', default: false},
      init: {type: 'boolean', short: 'i', default: false},
      'allow-local-line-sections': {type: 'boolean', short: 'a', default: false},
      quiet: {type: 'boolean', short: 'q', default: false},
      superquiet: {type: 'boolean', short: 's', default: false},
      'parse-ids': {type: 'boolean', default: false},
      'user-agent': {type: 'string', short: 'u'},
      help: {type: 'boolean', default: false},
    },
    allowPositionals: true,
  });

  if (values.help) {
    showHelp();
    process.exit(0);
  }

  if (values.init) {
    if (fs.existsSync('.unbroken_exclusions')) {
      console.error('.unbroken_exclusions already exists');
      process.exit(-1);
    } else {
      fs.writeFileSync('.unbroken_exclusions', '!node_modules');
      process.exit(0);
    }
  }

  const options: Options = {
    dir: positionals[0] || '.',
    exclusions: values.exclusions ?? '',
    'local-only': values['local-only']!,
    'allow-local-line-sections': values['allow-local-line-sections']!,
    quiet: values.quiet!,
    superquiet: values.superquiet!,
    'parse-ids': values['parse-ids']!,
    'user-agent': values['user-agent'] ?? '',
  };

  const c = new Checker(options);
  const n = await c.run();
  if (c.errors.length) {
    if (!options.superquiet) {
      console.log(`${n} errors, ${c.errors.length - n} warnings.`);
    }
  }
  process.exitCode = n;
}

void run();
