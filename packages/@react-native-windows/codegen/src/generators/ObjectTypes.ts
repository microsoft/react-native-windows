/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NativeModuleEnumDeclaration,
  NativeModuleBaseTypeAnnotation,
  NativeModuleUnionTypeAnnotation,
  NativeModuleStringTypeAnnotation,
  NativeModuleFunctionTypeAnnotation,
  UnsafeAnyTypeAnnotation,
  Nullable,
} from '@react-native/codegen/lib/CodegenSchema';
import {
  AliasMap,
  getAliasCppName,
  getAnonymousAliasCppName,
} from './AliasManaging';

export type CppStringTypes = 'std::string' | 'std::wstring';

export interface CppCodegenOptions {
  cppStringType: CppStringTypes;
}

function translateUnionReturnType(
  type: NativeModuleEnumDeclaration | NativeModuleUnionTypeAnnotation,
  options: CppCodegenOptions,
): string {
  const memberType = type.memberType;
  switch (type.memberType) {
    case 'StringTypeAnnotation':
      return options.cppStringType;
    case 'NumberTypeAnnotation':
      return 'double';
    case 'ObjectTypeAnnotation':
      return '::React::JSValue';
    default:
      throw new Error(
        `Unknown enum/union member type in translateReturnType: ${memberType}`,
      );
  }
}

export function translateFieldOrReturnType(
  type:
    | Nullable<
        | NativeModuleBaseTypeAnnotation
        | NativeModuleStringTypeAnnotation
        | NativeModuleFunctionTypeAnnotation
      >
    | UnsafeAnyTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  callerName: 'translateField' | 'translateReturnType',
  options: CppCodegenOptions,
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const returnType = type.type;
  switch (type.type) {
    case 'StringTypeAnnotation':
      return options.cppStringType;
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
    case 'DoubleTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'ArrayTypeAnnotation':
      if (type.elementType.type !== 'AnyTypeAnnotation') {
        return `std::vector<${translateFieldOrReturnType(
          type.elementType,
          aliases,
          `${baseAliasName}_element`,
          callerName,
          options,
        )}>`;
      } else {
        return `::React::JSValueArray`;
      }
    case 'GenericObjectTypeAnnotation':
      return '::React::JSValue';
    case 'ObjectTypeAnnotation':
      return getAnonymousAliasCppName(aliases, baseAliasName, type);
    case 'ReservedTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = type.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(`Unknown reserved function: ${name} in ${callerName}`);
      return 'double';
    }
    case 'TypeAliasTypeAnnotation':
      return getAliasCppName(type.name);
    case 'NullableTypeAnnotation':
      return `std::optional<${translateFieldOrReturnType(
        type.typeAnnotation,
        aliases,
        baseAliasName,
        callerName,
        options,
      )}>`;
    case 'FunctionTypeAnnotation':
    case 'MixedTypeAnnotation':
      return '';
    case 'EnumDeclaration':
    case 'UnionTypeAnnotation':
      return translateUnionReturnType(type, options);
    case 'AnyTypeAnnotation':
      return '::React::JSValue?';
    default:
      throw new Error(`Unhandled type in ${callerName}: ${returnType}`);
  }
}

export function translateField(
  type: Nullable<NativeModuleBaseTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
): string {
  return translateFieldOrReturnType(
    type,
    aliases,
    baseAliasName,
    'translateField',
    options,
  );
}
