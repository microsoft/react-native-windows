/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import yargs from 'yargs';
import {CodeGenOptions, runCodeGen} from './index';

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
  modulesTypeScriptTypes: {
    type: 'boolean',
    describe: 'generate turbo module definition files in TypeScript',
    default: false,
  },
  modulesCxx: {
    type: 'boolean',
    describe: 'generate C++ JSI turbo module spec files',
    default: false,
  },
  modulesWindows: {
    type: 'boolean',
    describe: 'generate turbo module spec files for REACT_MODULE',
    default: false,
  },
  methodOnly: {
    type: 'boolean',
    describe: 'generate only method metadata in C++ turbo module spec',
    default: false,
  },
  outputDirectory: {
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
  cppStringType: {
    choices: ['std::string', 'std::wstring'],
    describe:
      'C++ string type in generated code, should be "std::string" or "std::wstring"',
    default: 'std::string',
  },
  separateDataTypes: {
    type: 'boolean',
    describe: 'generate data types in a separate file',
    default: false,
  },
  componentsWindows: {
    type: 'boolean',
    describe: 'generate component cpp files for custom native components',
    default: false,
  },
  internalComponents: {
    type: 'boolean',
    describe:
      'generate non-ABI cpp/h for internal usage of built in native components [Only used within RNW itself]',
    default: false,
    hidden: true,
  },
}).argv;

if ((argv.file && argv.files) || (!argv.file && !argv.files)) {
  console.error('You must specify either --file or --files.');
  process.exit(1);
}

if (
  argv.cppStringType !== 'std::string' &&
  argv.cppStringType !== 'std::wstring'
) {
  console.error('cppStringType should be "std::string" or "std::wstring".');
  process.exit(1);
}

// type casting is necessary here because
// cppStringType does not become union of string literals
// until yargs.options get improved in the future
const changesNecessary = runCodeGen(<CodeGenOptions>argv);

if (argv.test && changesNecessary) {
  console.error(
    'There is a change in the output of codegen.  Rerun "npx @react-native-community/cli codegen-windows" to regenerate.',
  );
  process.exit(2);
}
