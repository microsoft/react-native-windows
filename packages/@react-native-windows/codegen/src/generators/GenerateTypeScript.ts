/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NamedShape,
  NativeModuleBaseTypeAnnotation,
  NativeModuleFunctionTypeAnnotation,
  NativeModuleObjectTypeAnnotation,
  NativeModuleParamTypeAnnotation,
  NativeModuleReturnTypeAnnotation,
  NativeModuleSchema,
  Nullable,
  SchemaType,
} from 'react-native-tscodegen';

interface CodegenNativeModuleSchema extends NativeModuleSchema {
  optionalTurboModule?: boolean;
}

export function setOptionalTurboModule(
  schema: NativeModuleSchema,
  optional: boolean,
): void {
  const cs = <CodegenNativeModuleSchema>schema;
  cs.optionalTurboModule = optional;
}

export function getOptionalTurboModule(schema: NativeModuleSchema): boolean {
  return (<CodegenNativeModuleSchema>schema).optionalTurboModule ?? false;
}

type ObjectProp = NamedShape<Nullable<NativeModuleBaseTypeAnnotation>>;
type FunctionParam = NamedShape<Nullable<NativeModuleParamTypeAnnotation>>;
type FunctionDecl = NamedShape<Nullable<NativeModuleFunctionTypeAnnotation>>;
type FilesOutput = Map<string, string>;

const moduleTemplate = `
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a TypeScript turbo module definition file.
 */

import {TurboModule, TurboModuleRegistry} from 'react-native';
'use strict';
::_MODULE_ALIASED_STRUCTS_::
export interface Spec extends TurboModule {
::_MODULE_MEMBERS_::
}

export default TurboModuleRegistry.::_MODULE_GETTER_::<Spec>('::_MODULE_NAME_::');
`;

function optionalSign<T>(obj: NamedShape<T>): string {
  return obj.optional ? '?' : '';
}

function translateType(
  type: Nullable<
    | NativeModuleBaseTypeAnnotation
    | NativeModuleParamTypeAnnotation
    | NativeModuleReturnTypeAnnotation
  >,
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const returnType = type.type;
  switch (type.type) {
    case 'StringTypeAnnotation':
      return 'string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
    case 'DoubleTypeAnnotation':
    case 'Int32TypeAnnotation':
      return 'number';
    case 'BooleanTypeAnnotation':
      return 'boolean';
    case 'ArrayTypeAnnotation':
      if (type.elementType) {
        return `${translateType(type.elementType)}[]`;
      } else {
        return `Array`;
      }
    case 'GenericObjectTypeAnnotation':
      return 'object';
    case 'ObjectTypeAnnotation':
      return `{${type.properties
        .map((prop: ObjectProp) => {
          return `${prop.name}${optionalSign(prop)}: ${translateType(
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
      return 'number';
    }
    case 'TypeAliasTypeAnnotation':
      return type.name;
    case 'NullableTypeAnnotation':
      return `(${translateType(type.typeAnnotation)} | null | undefined)`;
    case 'VoidTypeAnnotation':
      return `void`;
    case 'PromiseTypeAnnotation':
      return `Promise`;
    case `FunctionTypeAnnotation`:
      return `((${type.params
        .map((param: FunctionParam) => {
          return `${param.name}${optionalSign(param)}: ${translateType(
            param.typeAnnotation,
          )}`;
        })
        .join(', ')}) => ${translateType(type.returnTypeAnnotation)})`;
    default:
      throw new Error(`Unhandled type in translateReturnType: ${returnType}`);
  }
}

function translateAlias(
  name: string,
  type: NativeModuleObjectTypeAnnotation,
): string {
  return `
export interface ${name} {
${type.properties
  .map((prop: ObjectProp) => {
    return `  ${prop.name}${optionalSign(prop)}: ${translateType(
      prop.typeAnnotation,
    )};`;
  })
  .join('\n')}
}
`;
}

function tryGetConstantType(
  nativeModule: NativeModuleSchema,
): NativeModuleObjectTypeAnnotation | undefined {
  const candidates = nativeModule.spec.properties.filter(
    prop => prop.name === 'getConstants',
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

function translateMethod(func: FunctionDecl): string {
  const funcType =
    func.typeAnnotation.type === 'NullableTypeAnnotation'
      ? func.typeAnnotation.typeAnnotation
      : func.typeAnnotation;

  return `
  ${func.name}(${funcType.params
    .map((param: FunctionParam) => {
      return `${param.name}${optionalSign(param)}: ${translateType(
        param.typeAnnotation,
      )}`;
    })
    .join(', ')})${optionalSign(func)}: ${translateType(
    funcType.returnTypeAnnotation,
  )}${
    funcType.returnTypeAnnotation.type === 'ObjectTypeAnnotation' ? '' : ';'
  }`;
}

export function generateTypeScript(
  _libraryName: string,
  schema: SchemaType,
  _moduleSpecName: string,
): FilesOutput {
  const files = new Map<string, string>();

  for (const moduleName of Object.keys(schema.modules)) {
    const nativeModule = schema.modules[moduleName];
    // from 0.65 facebook's react-native-codegen
    // the module name has the Native prefix comparing to 0.63
    // when reading files we provided
    const nativePrefix = 'Native';
    const preferredModuleName = moduleName.startsWith(nativePrefix)
      ? moduleName.substr(nativePrefix.length)
      : moduleName;

    if (nativeModule.type === 'NativeModule') {
      console.log(`Generating ${preferredModuleName}Spec.g.ts`);

      const aliasCode = Object.keys(nativeModule.aliases)
        .map(name => translateAlias(name, nativeModule.aliases[name]))
        .join('');

      const constantType = tryGetConstantType(nativeModule);
      const constantCode =
        constantType === undefined
          ? ''
          : `  getConstants(): ${translateType(constantType)}`;

      const methods = nativeModule.spec.properties.filter(
        prop => prop.name !== 'getConstants',
      );
      const membersCode = methods.map(translateMethod).join('');

      files.set(
        `${preferredModuleName}Spec.g.ts`,
        moduleTemplate
          .replace(/::_MODULE_ALIASED_STRUCTS_::/g, aliasCode)
          .replace(/::_MODULE_MEMBERS_::/g, constantCode + membersCode)
          .replace(/::_MODULE_NAME_::/g, preferredModuleName)
          .replace(
            /::_MODULE_GETTER_::/g,
            getOptionalTurboModule(nativeModule) ? 'get' : 'getEnforcing',
          ),
      );
    }
  }

  return files;
}
