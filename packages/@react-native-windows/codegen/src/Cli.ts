/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import yargs from 'yargs';
import path from 'path';
import fs from '@react-native-windows/fs';
import globby from 'globby';
import {createNM2Generator} from './generators/GenerateNM2';
import {generateTypeScript} from './generators/GenerateTypeScript';
// @ts-ignore
import {parseFile} from 'react-native-tscodegen/lib/rncodegen/src/parsers/flow';
// @ts-ignore
import schemaValidator from 'react-native-tscodegen/lib/rncodegen/src/schemaValidator';

const argv = yargs.options({
  file: {
    type: 'string',
    describe: 'file which contains spec',
  },
  files: {
    type: 'array',
    describe: 'glob patterns for files which contains specs',
  },
  ts: {
    type: 'boolean',
    describe: 'generate turbo module definition files in TypeScript',
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

function normalizeFileMap(
  map: Map<string, string>,
  outputDir: string,
  outMap: Map<string, string>,
): void {
  for (const [fileName, contents] of map) {
    const location = path.join(outputDir, fileName);
    outMap.set(path.normalize(location), contents);
  }
}

function checkFilesForChanges(
  map: Map<string, string>,
  outputDir: string,
): boolean {
  let hasChanges = false;

  const allExistingFiles = globby
    .sync(`${outputDir}/**`)
    .map((_) => path.normalize(_))
    .sort();
  const allGeneratedFiles = [...map.keys()]
    .map((_) => path.normalize(_))
    .sort();

  if (
    allExistingFiles.length !== allGeneratedFiles.length ||
    !allExistingFiles.every((val, index) => val === allGeneratedFiles[index])
  )
    return true;

  for (const [fileName, contents] of map) {
    if (!fs.existsSync(fileName)) {
      hasChanges = true;
      continue;
    }

    const currentContents = fs.readFileSync(fileName, 'utf8');
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

  // This ensures that we delete any generated files from modules that have been deleted
  const allExistingFiles = globby.sync(`${outputDir}/**`);
  allExistingFiles.forEach((existingFile) => {
    if (!map.has(path.normalize(existingFile))) {
      fs.unlinkSync(existingFile);
    }
  });

  for (const [fileName, contents] of map) {
    try {
      fs.mkdirSync(path.dirname(fileName), {recursive: true});

      if (fs.existsSync(fileName)) {
        const currentContents = fs.readFileSync(fileName, 'utf8');
        // Don't update the files if there are no changes as this breaks incremental builds
        if (currentContents === contents) {
          continue;
        }
      }

      fs.writeFileSync(fileName, contents);
    } catch (error) {
      success = false;
      console.error(`Failed to write ${fileName} to ${fileName}`, error);
    }
  }

  return success;
}

function parseFlowFile(filename: string): SchemaType {
  try {
    return parseFile(filename);
  } catch (e) {
    if (e instanceof Error) {
      e.message = `(${filename}): ${e.message}`;
    }
    throw e;
  }
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
        const schema = parseFlowFile(filename);
        merged.modules = {...merged.modules, ...schema.modules};
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

  const componentOutputdir = path.join(
    outputDirectory,
    'react/components',
    libraryName,
  );

  const generatedFiles = new Map<string, string>();

  generatedFiles.set(
    path.join(outputDirectory, '.clang-format'),
    'DisableFormat: true\nSortIncludes: false',
  );

  const generateNM2 = createNM2Generator({namespace: argv.namespace});

  const generatorPropsH =
    require('react-native-tscodegen/lib/rncodegen/src/generators/components/GeneratePropsH').generate;
  const generatorPropsCPP =
    require('react-native-tscodegen/lib/rncodegen/src/generators/components/GeneratePropsCPP').generate;
  const generatorShadowNodeH =
    require('react-native-tscodegen/lib/rncodegen/src/generators/components/GenerateShadowNodeH').generate;
  const generatorShadowNodeCPP =
    require('react-native-tscodegen/lib/rncodegen/src/generators/components/GenerateShadowNodeCPP').generate;
  const generatorComponentDescriptorH =
    require('react-native-tscodegen/lib/rncodegen/src/generators/components/GenerateComponentDescriptorH').generate;
  const generatorEventEmitterH =
    require('react-native-tscodegen/lib/rncodegen/src/generators/components/GenerateEventEmitterH').generate;
  const generatorEventEmitterCPP =
    require('react-native-tscodegen/lib/rncodegen/src/generators/components/GenerateEventEmitterCpp').generate;

  normalizeFileMap(
    generateNM2(libraryName, schema, moduleSpecName),
    outputDirectory,
    generatedFiles,
  );

  if (argv.ts) {
    normalizeFileMap(
      generateTypeScript(libraryName, schema, moduleSpecName),
      outputDirectory,
      generatedFiles,
    );
  }

  if (
    Object.keys(schema.modules).some(
      (moduleName) => schema.modules[moduleName].type === 'Component',
    )
  ) {
    const componentGenerators = [
      generatorPropsH,
      generatorPropsCPP,
      generatorShadowNodeH,
      generatorShadowNodeCPP,
      generatorComponentDescriptorH,
      generatorEventEmitterH,
      generatorEventEmitterCPP,
    ];

    componentGenerators.forEach((generator) => {
      const generated: Map<string, string> = generator(
        libraryName,
        schema,
        moduleSpecName,
      );
      normalizeFileMap(generated, componentOutputdir, generatedFiles);
    });
  }

  if (test === true) {
    return checkFilesForChanges(generatedFiles, outputDirectory);
  }

  return writeMapToFiles(generatedFiles, outputDirectory);
}

if ((argv.file && argv.files) || (!argv.file && !argv.files)) {
  console.error('You must specify either --file or --files.');
  process.exit(1);
}

let schema: SchemaType;
if (argv.file) {
  schema = parseFlowFile(argv.file);
} else {
  schema = combineSchemas(globby.sync(argv.files as string[]));
}

const libraryName = argv.libraryName;
const moduleSpecName = 'moduleSpecName';
const outputDirectory = argv.outdir;
generate(
  {libraryName, schema, outputDirectory, moduleSpecName},
  {generators: [], test: false},
);
