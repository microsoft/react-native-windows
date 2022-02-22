/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {
  NamedShape,
  NativeModuleBaseTypeAnnotation,
  NativeModuleObjectTypeAnnotation,
  NativeModuleSchema,
  Nullable,
  SchemaType,
} from 'react-native-tscodegen';

type ObjectProp = NamedShape<Nullable<NativeModuleBaseTypeAnnotation>>;
type FilesOutput = Map<string, string>;

const moduleTemplate = `
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a TypeScript turbo module definition file.
 */

import {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import * as TurboModuleRegistry from 'react-native/Libraries/TurboModule/TurboModuleRegistry';
import {Int32, Float, Double, RootTag} from 'react-native/Libraries/Types/CodegenTypes';
'use strict';

::_MODULE_ALIASED_STRUCTS_::
export interface Spec extends TurboModule {
::_MODULE_MEMBERS_::
}

export default TurboModuleRegistry.getEnforcing<Spec>('::_MODULE_NAME_::');
`;

function translateType(type: Nullable<NativeModuleBaseTypeAnnotation>): string {
  // avoid: Property 'type' does not exist on type 'never'
  const returnType = type.type;
  switch (type.type) {
    case 'StringTypeAnnotation':
      return 'string';
    case 'NumberTypeAnnotation':
      return 'number';
    case 'FloatTypeAnnotation':
      return 'Float';
    case 'DoubleTypeAnnotation':
      return 'Double';
    case 'Int32TypeAnnotation':
      return 'Int32';
    case 'BooleanTypeAnnotation':
      return 'boolean';
    case 'ArrayTypeAnnotation':
      if (type.elementType) {
        return `${translateType(type.elementType)}[]>`;
      } else {
        return `Array`;
      }
    case 'GenericObjectTypeAnnotation':
      return 'object';
    case 'ObjectTypeAnnotation':
      return `{${type.properties
        .map((prop: ObjectProp) => {
          return `${prop.name}${prop.optional ? '?' : ''}: ${translateType(
            prop.typeAnnotation,
          )}`;
        })
        .join(', ')}}`;
    case 'ReservedTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = type.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${name} in translateReturnType`,
        );
      return 'RootTag';
    }
    case 'TypeAliasTypeAnnotation':
      return type.name;
    case 'NullableTypeAnnotation':
      return `(${translateType(type.typeAnnotation)} | null | undefined)`;
    default:
      throw new Error(`Unhandled type in translateReturnType: ${returnType}`);
  }
}

function translateAlias(
  name: string,
  type: NativeModuleObjectTypeAnnotation,
): string {
  return `interface ${name} {
${type.properties
  .map((prop: ObjectProp) => {
    return `  ${prop.name}${prop.optional ? '?' : ''}: ${translateType(
      prop.typeAnnotation,
    )};`;
  })
  .join('\n')}
}`;
}

export function tryGetConstantType(
  nativeModule: NativeModuleSchema,
): NativeModuleObjectTypeAnnotation | undefined {
  const candidates = nativeModule.spec.properties.filter(
    (prop) => prop.name === 'getConstants',
  );
  if (candidates.length === 0) {
    return undefined;
  }

  const getConstant = candidates[0];
  const funcType =
    getConstant.typeAnnotation.type === 'NullableTypeAnnotation'
      ? getConstant.typeAnnotation.typeAnnotation
      : getConstant.typeAnnotation;
  if (
    funcType.params.length > 0 ||
    funcType.returnTypeAnnotation.type !== 'ObjectTypeAnnotation'
  ) {
    return undefined;
  }

  const constantType = funcType.returnTypeAnnotation;
  if (constantType.properties.length === 0) {
    return undefined;
  }

  return constantType;
}

export function createTypeScriptGenerator() {
  return (
    _libraryName: string,
    schema: SchemaType,
    _moduleSpecName: string,
  ): FilesOutput => {
    const files = new Map<string, string>();

    for (const moduleName of Object.keys(schema.modules)) {
      const nativeModule = schema.modules[moduleName];
      // from 0.65 facebook's react-native-codegen
      // the module name has the Native prefix comparing to 0.63
      // when reading files we provided
      const preferredModuleName = moduleName.startsWith('Native')
        ? moduleName.substr(6)
        : moduleName;

      if (nativeModule.type === 'NativeModule') {
        console.log(`Generating ${preferredModuleName}Spec.g.ts`);

        const aliasCode = Object.keys(nativeModule.aliases)
          .map((name) => translateAlias(name, nativeModule.aliases[name]))
          .join('\n\n');

        const constantType = tryGetConstantType(nativeModule);
        const constantCode =
          constantType === undefined ? '' : `  getConstants(): ${constantType}`;

        const membersCode = '';

        files.set(
          `${preferredModuleName}Spec.g.ts`,
          moduleTemplate
            .replace(/::_MODULE_ALIASED_STRUCTS_::/g, aliasCode)
            .replace(/::_MODULE_MEMBERS_::/g, constantCode + membersCode)
            .replace(/::_MODULE_NAME_::/g, preferredModuleName),
        );
      }
    }

    return files;
  };
}
