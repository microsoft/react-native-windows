/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import yargs from 'yargs';
import {runCodeGen} from './index';

const argv = yargs.options({
  file: {
    type: 'string',
    describe: 'file which contains spec',
  },
  files: {
    type: 'string',
    array: true,
    describe: 'glob patterns for files which contains specs',
  },
  ts: {
    type: 'boolean',
    describe: 'generate turbo module definition files in TypeScript',
    default: false,
  },
  methodonly: {
    type: 'boolean',
    describe: 'generate only method metadata in C++ turbo module spec',
    default: false,
  },
  outdir: {
    type: 'string',
    describe: 'output directory',
    default: 'codegen',
  },
  test: {
    type: 'boolean',
    describe: 'Verify that the generated output is unchanged',
    default: false,
  },
  namespace: {
    type: 'string',
    describe: 'C++/C# Namespace to put generated native modules in',
    default: 'MyNamespace',
  },
  libraryName: {
    type: 'string',
    required: true,
    describe: 'Used for part of the path generated within the codegen dir',
  },
}).argv;

if ((argv.file && argv.files) || (!argv.file && !argv.files)) {
  console.error('You must specify either --file or --files.');
  process.exit(1);
}

runCodeGen(argv);
