/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {NativeModuleSchema} from '@react-native/codegen/lib/CodegenSchema';
import {AliasMap, getAnonymousAliasCppName} from './AliasManaging';

export function generateValidateConstants(
  nativeModule: NativeModuleSchema,
  aliases: AliasMap,
): [string, string] | undefined {
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

  const cppName = getAnonymousAliasCppName(aliases, 'Constants', constantType);

  return [
    `      TypedConstant<${cppName}>{0},`,
    `    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "${cppName}",
          "    REACT_GET_CONSTANTS(GetConstants) ${cppName} GetConstants() noexcept {/*implementation*/}\\n"
          "    REACT_GET_CONSTANTS(GetConstants) static ${cppName} GetConstants() noexcept {/*implementation*/}\\n");`,
  ];
}
