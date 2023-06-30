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
import {CppCodegenOptions, translateFieldOrReturnType} from './ObjectTypes';

function translateReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
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
        options,
      )}>`;
    default:
      return translateFieldOrReturnType(
        type,
        aliases,
        baseAliasName,
        'translateReturnType',
        options,
      );
  }
}

export function translateSpecReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
) {
  return translateReturnType(
    type,
    aliases,
    `${baseAliasName}_returnType`,
    options,
  );
}

export function translateImplReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
) {
  return translateReturnType(
    type,
    aliases,
    `${baseAliasName}_returnType`,
    options,
  );
}
