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
const FlowParser = require(path.resolve(rncodegenPath, 'lib/parsers/flow'));
const TypeScriptParser = require(path.resolve(
  rncodegenPath,
  'lib/parsers/typescript',
));

const schemaValidator = require(path.resolve(
  rncodegenPath,
  'lib/SchemaValidator',
));

interface Options {
  libraryName: string;
  methodOnly: boolean;
  modulesCxx: boolean;
  moduleSpecName: string;
  modulesTypeScriptTypes: boolean;
  modulesWindows: boolean;
  namespace: string;
  outputDirectory: string;
  schema: SchemaType;
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

export function parseFile(filename: string): SchemaType {
  try {
    const isTypeScript =
      path.extname(filename) === '.ts' || path.extname(filename) === '.tsx';
    const contents = fs.readFileSync(filename, 'utf8');
    const schema = isTypeScript
      ? TypeScriptParser.parseString(contents, filename)
      : FlowParser.parseString(contents, filename);
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
        const schema = parseFile(filename);
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
    methodOnly,
    modulesCxx,
    moduleSpecName,
    modulesTypeScriptTypes,
    modulesWindows,
    namespace,
    outputDirectory,
    schema,
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
    methodOnly,
    namespace,
  });

  const generateJsiModuleH = require(path.resolve(
    rncodegenPath,
    'lib/generators/modules/GenerateModuleH',
  )).generate;
  const generateJsiModuleCpp = require(path.resolve(
    rncodegenPath,
    'lib/generators/modules/GenerateModuleCpp',
  )).generate;
  const generatorPropsH = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GeneratePropsH',
  )).generate;
  const generatorPropsCPP = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GeneratePropsCPP',
  )).generate;
  const generatorShadowNodeH = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GenerateShadowNodeH',
  )).generate;
  const generatorShadowNodeCPP = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GenerateShadowNodeCPP',
  )).generate;
  const generatorComponentDescriptorH = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GenerateComponentDescriptorH',
  )).generate;
  const generatorEventEmitterH = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GenerateEventEmitterH',
  )).generate;
  const generatorEventEmitterCPP = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GenerateEventEmitterCpp',
  )).generate;
  const generatorStateCPP = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GenerateStateCpp',
  )).generate;
  const generatorStateH = require(path.resolve(
    rncodegenPath,
    'lib/generators/components/GenerateStateH',
  )).generate;

  const moduleGenerators = [];

  if (modulesWindows) {
    moduleGenerators.push(generateNM2);
  }

  if (modulesCxx) {
    moduleGenerators.push(generateJsiModuleH);
    moduleGenerators.push(generateJsiModuleCpp);
  }

  if (modulesTypeScriptTypes) {
    moduleGenerators.push(generateTypeScript);
  }

  moduleGenerators.forEach(generator => {
    const generated: Map<string, string> = generator(
      libraryName,
      schema,
      moduleSpecName,
    );
    normalizeFileMap(generated, outputDirectory, generatedFiles);
  });

  if (
    Object.keys(schema.modules).some(
      moduleName => schema.modules[moduleName].type === 'Component',
    )
  ) {
    const componentGenerators = [
      generatorComponentDescriptorH,
      generatorEventEmitterCPP,
      generatorEventEmitterH,
      generatorPropsCPP,
      generatorPropsH,
      generatorShadowNodeCPP,
      generatorShadowNodeH,
      generatorStateCPP,
      generatorStateH,
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
  methodOnly: boolean;
  modulesCxx: boolean;
  modulesTypeScriptTypes: boolean;
  modulesWindows: boolean;
  namespace: string;
  outputDirectory: string;
  test: boolean;
};

export function runCodeGen(options: CodeGenOptions): boolean {
  if (!options.file && !options.files)
    throw new Error('Must specify file or files option');

  const schema = options.file
    ? parseFile(options.file)
    : combineSchemas(globby.sync(options.files!));

  const libraryName = options.libraryName;
  const moduleSpecName = 'moduleSpecName';
  const {
    methodOnly,
    modulesCxx,
    modulesTypeScriptTypes,
    modulesWindows,
    namespace,
    outputDirectory,
  } = options;
  return generate(
    {
      libraryName,
      methodOnly,
      modulesCxx,
      moduleSpecName,
      modulesTypeScriptTypes,
      modulesWindows,
      namespace,
      outputDirectory,
      schema,
    },
    {generators: [], test: options.test},
  );
}
