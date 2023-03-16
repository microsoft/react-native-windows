/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NativeModuleReturnTypeAnnotation,
  Nullable,
} from '@react-native/codegen/lib/CodegenSchema';
import {AliasMap} from './AliasManaging';
import {translateFieldOrReturnType} from './ObjectTypes';

function translateReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
): string {
  switch (type.type) {
    case 'VoidTypeAnnotation':
    case 'PromiseTypeAnnotation':
      return 'void';
    case 'NullableTypeAnnotation':
      return `std::optional<${translateReturnType(
        type.typeAnnotation,
        aliases,
        baseAliasName,
      )}>`;
    default:
      return translateFieldOrReturnType(
        type,
        aliases,
        baseAliasName,
        'translateReturnType',
      );
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
