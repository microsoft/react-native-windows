/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NativeModuleFunctionTypeAnnotation,
  NativeModulePropertyShape,
  NativeModuleSchema,
} from 'react-native-tscodegen';
import {AliasMap} from './AliasManaging';
import {translateArgs, translateSpecArgs} from './ParamTypes';
import {translateImplReturnType, translateSpecReturnType} from './ReturnTypes';

function isMethodSync(funcType: NativeModuleFunctionTypeAnnotation) {
  return (
    funcType.returnTypeAnnotation.type !== 'VoidTypeAnnotation' &&
    funcType.returnTypeAnnotation.type !== 'PromiseTypeAnnotation'
  );
}

function isMethodReturnPromise(funcType: NativeModuleFunctionTypeAnnotation) {
  return funcType.returnTypeAnnotation.type === 'PromiseTypeAnnotation';
}

function getPossibleMethodSignatures(
  prop: NativeModulePropertyShape,
  funcType: NativeModuleFunctionTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
): string[] {
  const args = translateArgs(funcType.params, aliases, baseAliasName);
  if (isMethodReturnPromise(funcType)) {
    // TODO: type of the promise could be provided in the future
    args.push('::React::ReactPromise<::React::JSValue> &&result');
  }

  // TODO: be much more exhastive on the possible method signatures that can be used..
  const sig = `REACT_${isMethodSync(funcType) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) ${translateImplReturnType(
    funcType.returnTypeAnnotation,
    aliases,
    baseAliasName,
  )} ${prop.name}(${args.join(', ')}) noexcept { /* implementation */ }`;

  const staticsig = `REACT_${isMethodSync(funcType) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) static ${translateImplReturnType(
    funcType.returnTypeAnnotation,
    aliases,
    baseAliasName,
  )} ${prop.name}(${args.join(', ')}) noexcept { /* implementation */ }`;

  return [sig, staticsig];
}

function translatePossibleMethodSignatures(
  prop: NativeModulePropertyShape,
  funcType: NativeModuleFunctionTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
): string {
  return getPossibleMethodSignatures(prop, funcType, aliases, baseAliasName)
    .map(sig => `"    ${sig}\\n"`)
    .join('\n          ');
}

function renderProperties(
  properties: ReadonlyArray<NativeModulePropertyShape>,
  aliases: AliasMap,
  tuple: boolean,
): string {
  // TODO: generate code for constants
  return properties
    .filter(prop => prop.name !== 'getConstants')
    .map((prop, index) => {
      // TODO: prop.optional === true
      // TODO: prop.typeAnnotation.type === 'NullableTypeAnnotation'
      const propAliasName = prop.name;
      const funcType =
        prop.typeAnnotation.type === 'NullableTypeAnnotation'
          ? prop.typeAnnotation.typeAnnotation
          : prop.typeAnnotation;

      const traversedArgs = translateSpecArgs(
        funcType.params,
        aliases,
        propAliasName,
      );

      const translatedReturnParam = translateSpecReturnType(
        funcType.returnTypeAnnotation,
        aliases,
        propAliasName,
      );

      if (isMethodReturnPromise(funcType)) {
        // TODO: type of the promise could be provided in the future
        traversedArgs.push('Promise<::React::JSValue>');
      }

      if (tuple) {
        return `      ${
          isMethodSync(funcType) ? 'Sync' : ''
        }Method<${translatedReturnParam}(${traversedArgs.join(
          ', ',
        )}) noexcept>{${index}, L"${prop.name}"},`;
      } else {
        return `    REACT_SHOW_METHOD_SPEC_ERRORS(
          ${index},
          "${prop.name}",
          ${translatePossibleMethodSignatures(
            prop,
            funcType,
            aliases,
            propAliasName,
          )});`;
      }
    })
    .join('\n');
}

export function generateValidateMethods(
  nativeModule: NativeModuleSchema,
  aliases: AliasMap,
): [string, string] {
  const properties = nativeModule.spec.properties;
  const traversedProperties = renderProperties(properties, aliases, false);
  const traversedPropertyTuples = renderProperties(properties, aliases, true);
  return [traversedPropertyTuples, traversedProperties];
}
