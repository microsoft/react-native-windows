/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as yargs from 'yargs';
import * as path from 'path';
import * as fs from 'fs';
import * as globby from 'globby';
import {createNM2Generator} from './generators/GenerateNM2';
// @ts-ignore
import {parseFile} from 'react-native-tscodegen/lib/rncodegen/src/parsers/flow';
// @ts-ignore
import * as schemaValidator from 'react-native-tscodegen/lib/rncodegen/src/schemaValidator';

const argv = yargs.options({
  file: {
    type: 'string',
    describe: 'file which contains spec',
  },
  files: {
    type: 'array',
    describe: 'glob patterns for files which contains specs',
  },
  test: {
    type: 'boolean',
    describe: 'Verify that the generated output is unchanged',
  },
  namespace: {
    type: 'string',
    describe: 'C++/C# Namespace to put generated native modules in',
    default: 'MyNamespace',
  },
}).argv;

import {SchemaType} from 'react-native-tscodegen';

interface Options {
  libraryName: string;
  schema: SchemaType;
  outputDirectory: string;
  moduleSpecName: string;
}

interface Config {
  generators: any[] /*Generators[]*/;
  test?: boolean;
}

/*
const GENERATORS = {
  descriptors: [generateComponentDescriptorH.generate],
  events: [
    generateEventEmitterCpp.generate,
    generateEventEmitterH.generate,
    generateModuleHObjCpp.generate,
    generateModuleMm.generate,
  ],
  props: [
    generateComponentHObjCpp.generate,
    generatePropsCpp.generate,
    generatePropsH.generate,
    generatePropsJavaInterface.generate,
    generatePropsJavaDelegate.generate,
  ],
  modules: [generateModuleCpp.generate, generateModuleH.generate],
  tests: [generateTests.generate],
  'shadow-nodes': [
    generateShadowNodeCpp.generate,
    generateShadowNodeH.generate,
  ],
};
*/

function checkFilesForChanges(
  map: Map<string, string>,
  outputDir: string,
): boolean {
  let hasChanges = false;

  for (const [contents, fileName] of map) {
    const location = path.join(outputDir, fileName);
    if (!fs.existsSync(location)) {
      hasChanges = true;
      continue;
    }

    const currentContents = fs.readFileSync(location, 'utf8');
    if (currentContents !== contents) {
      console.error(`- ${fileName} has changed`);
      hasChanges = true;
      continue;
    }
  }

  return hasChanges;
}

function writeMapToFiles(map: Map<string, string>, outputDir: string) {
  let success = true;
  map.forEach((contents: string, fileName: string) => {
    try {
      const location = path.join(outputDir, fileName);
      fs.mkdirSync(path.dirname(location), {recursive: true});
      fs.writeFileSync(location, contents);
    } catch (error) {
      success = false;
      console.error(`Failed to write ${fileName} to ${outputDir}`, error);
    }
  });

  return success;
}

function combineSchemas(files: string[]): SchemaType {
  return files.reduce(
    (merged, filename) => {
      const contents = fs.readFileSync(filename, 'utf8');
      if (
        contents &&
        (/export\s+default\s+\(?codegenNativeComponent</.test(contents) ||
          contents.includes('extends TurboModule'))
      ) {
        const schema = parseFile(filename);

        if (schema && schema.modules) {
          merged.modules = {...merged.modules, ...schema.modules};
        }
      }
      return merged;
    },
    {modules: {}},
  );
}

function generate(
  {libraryName, schema, outputDirectory, moduleSpecName}: Options,
  {/*generators,*/ test}: Config,
): boolean {
  schemaValidator.validate(schema);

  const generatedFiles = [];
  /*
  for (const name of generators) {
    for (const generator of GENERATORS[name]) {
      generatedFiles.push(...generator(libraryName, schema, moduleSpecName));
    }
  }
*/

  const generateNM2 = createNM2Generator({namespace: argv.namespace});

  generatedFiles.push(...generateNM2(libraryName, schema, moduleSpecName));

  const filesToUpdate = new Map<string, string>([...generatedFiles]);

  if (test === true) {
    return checkFilesForChanges(filesToUpdate, outputDirectory);
  }

  return writeMapToFiles(filesToUpdate, outputDirectory);
}

if ((argv.file && argv.files) || (!argv.file && !argv.files)) {
  console.error('You must specify either --file or --files.');
  process.exit(1);
}

let schema: SchemaType;
if (argv.file) {
  schema = parseFile(argv.file);
} else {
  schema = combineSchemas(globby.sync(argv.files as string[]));
}

const libraryName = 'libraryName';
const moduleSpecName = 'moduleSpecName';
const outputDirectory = 'codegen';
generate(
  {libraryName, schema, outputDirectory, moduleSpecName},
  {generators: [], test: false},
);
