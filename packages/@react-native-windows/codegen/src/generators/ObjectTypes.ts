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
  Nullable,
} from '@react-native/codegen/lib/CodegenSchema';
import {
  AliasMap,
  getAliasCppName,
  getAnonymousAliasCppName,
} from './AliasManaging';

function translateUnionReturnType(
  type: NativeModuleEnumDeclaration | NativeModuleUnionTypeAnnotation,
): string {
  const memberType = type.memberType;
  switch (type.memberType) {
    case 'StringTypeAnnotation':
      return 'std::string';
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
  type: Nullable<NativeModuleBaseTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
  callerName: 'translateField' | 'translateReturnType',
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const returnType = type.type;
  switch (type.type) {
    case 'StringTypeAnnotation':
      return 'std::string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
    case 'DoubleTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'ArrayTypeAnnotation':
      if (type.elementType) {
        return `std::vector<${translateFieldOrReturnType(
          type.elementType,
          aliases,
          `${baseAliasName}_element`,
          callerName,
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
      )}>`;
    case 'EnumDeclaration':
    case 'UnionTypeAnnotation':
      return translateUnionReturnType(type);
    default:
      throw new Error(`Unhandled type in ${callerName}: ${returnType}`);
  }
}

export function translateField(
  type: Nullable<NativeModuleBaseTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
): string {
  return translateFieldOrReturnType(
    type,
    aliases,
    baseAliasName,
    'translateField',
  );
}
