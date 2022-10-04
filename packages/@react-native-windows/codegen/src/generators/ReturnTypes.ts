/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NativeModuleReturnTypeAnnotation,
  Nullable,
} from 'react-native-tscodegen';
import {
  AliasMap,
  getAliasCppName,
  getAnonymousAliasCppName,
} from './AliasManaging';

function translateReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const returnType = type.type;
  switch (type.type) {
    case 'VoidTypeAnnotation':
    case 'PromiseTypeAnnotation':
      return 'void';
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
        return `std::vector<${translateReturnType(
          type.elementType,
          aliases,
          `${baseAliasName}_element`,
        )}>`;
      } else {
        return '::React::JSValueArray';
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
        throw new Error(
          `Unknown reserved function: ${name} in translateReturnType`,
        );
      return 'double';
    }
    case 'TypeAliasTypeAnnotation':
      return getAliasCppName(type.name);
    case 'NullableTypeAnnotation':
      return `std::optional<${translateReturnType(
        type.typeAnnotation,
        aliases,
        baseAliasName,
      )}>`;
    default:
      throw new Error(`Unhandled type in translateReturnType: ${returnType}`);
  }
}

export function translateSpecReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
) {
  return translateReturnType(type, aliases, `${baseAliasName}_returnType`);
}

export function translateImplReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
) {
  return translateReturnType(type, aliases, `${baseAliasName}_returnType`);
}
