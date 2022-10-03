/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import path from 'path';
import fs from '@react-native-windows/fs';
import globby from 'globby';
import {createNM2Generator} from './generators/GenerateNM2';
import {
  generateTypeScript,
  setOptionalTurboModule,
} from './generators/GenerateTypeScript';
import type {SchemaType} from 'react-native-tscodegen';

// Load react-native-codegen from react-native
const rnPath = path.dirname(require.resolve('react-native/package.json'));
const rncodegenPath = path.dirname(
  require.resolve('react-native-codegen/package.json', {paths: [rnPath]}),
);
const {parseFile} = require(path.resolve(rncodegenPath, 'lib/parsers/flow'));
const schemaValidator = require(path.resolve(
  rncodegenPath,
  'lib/schemaValidator',
));

interface Options {
  libraryName: string;
  schema: SchemaType;
  outputDirectory: string;
  moduleSpecName: string;
  namespace: string;
  methodonly: boolean;
  ts: boolean;
}

interface Config {
  generators: any[] /*Generators[]*/;
  test?: boolean;
}

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
    .map(_ => path.normalize(_))
    .sort();
  const allGeneratedFiles = [...map.keys()].map(_ => path.normalize(_)).sort();

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
      console.log(`- ${fileName} has changed`);
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
  allExistingFiles.forEach(existingFile => {
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

export function parseFlowFile(filename: string): SchemaType {
  try {
    const schema = parseFile(filename);
    // there will be at most one turbo module per file
    const moduleName = Object.keys(schema.modules)[0];
    if (moduleName) {
      const spec = schema.modules[moduleName];
      if (spec.type === 'NativeModule') {
        const contents = fs.readFileSync(filename, 'utf8');
        if (contents) {
          // This is a temporary implementation until such information is added to the schema in facebook/react-native
          if (contents.includes('TurboModuleRegistry.get<')) {
            setOptionalTurboModule(spec, true);
          } else if (contents.includes('TurboModuleRegistry.getEnforcing<')) {
            setOptionalTurboModule(spec, false);
          }
        }
      }
    }
    return schema;
  } catch (e) {
    if (e instanceof Error) {
      e.message = `(${filename}): ${e.message}`;
    }
    throw e;
  }
}

export function combineSchemas(files: string[]): SchemaType {
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

export function generate(
  {
    libraryName,
    schema,
    outputDirectory,
    moduleSpecName,
    namespace,
    methodonly,
    ts,
  }: Options,
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

  const generateNM2 = createNM2Generator({
    namespace,
    methodonly,
  });

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

  if (ts) {
    normalizeFileMap(
      generateTypeScript(libraryName, schema, moduleSpecName),
      outputDirectory,
      generatedFiles,
    );
  }

  if (
    Object.keys(schema.modules).some(
      moduleName => schema.modules[moduleName].type === 'Component',
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

    componentGenerators.forEach(generator => {
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

export type CodeGenOptions = {
  file?: string;
  files?: string[];
  libraryName: string;
  outdir: string;
  namespace: string;
  methodonly: boolean;
  ts: boolean;
  test: boolean;
};

export function runCodeGen(options: CodeGenOptions): boolean {
  if (!options.file && !options.files)
    throw new Error('Must specify file or files option');

  const schema = options.file
    ? parseFlowFile(options.file)
    : combineSchemas(globby.sync(options.files!));

  const libraryName = options.libraryName;
  const moduleSpecName = 'moduleSpecName';
  const outputDirectory = options.outdir;
  const {namespace, methodonly, ts} = options;
  return generate(
    {
      libraryName,
      schema,
      outputDirectory,
      moduleSpecName,
      namespace,
      methodonly,
      ts,
    },
    {generators: [], test: options.test},
  );
}
