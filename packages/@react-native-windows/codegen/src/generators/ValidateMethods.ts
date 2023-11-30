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
} from '@react-native/codegen/lib/CodegenSchema';
import {AliasMap} from './AliasManaging';
import type {CppCodegenOptions} from './ObjectTypes';
import {translateArgs, translateSpecArgs} from './ParamTypes';
import {translateImplReturnType, translateSpecReturnType} from './ReturnTypes';

function isMethodSync(funcType: NativeModuleFunctionTypeAnnotation) {
  return (
    funcType.returnTypeAnnotation.type !== 'VoidTypeAnnotation' &&
    funcType.returnTypeAnnotation.type !== 'PromiseTypeAnnotation'
  );
}

function getPossibleMethodSignatures(
  prop: NativeModulePropertyShape,
  funcType: NativeModuleFunctionTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
): string[] {
  const args = translateArgs(funcType.params, aliases, baseAliasName, options);
  if (funcType.returnTypeAnnotation.type === 'PromiseTypeAnnotation') {
    if (funcType.returnTypeAnnotation.elementType) {
      args.push(
        `::React::ReactPromise<${translateImplReturnType(
          funcType.returnTypeAnnotation.elementType,
          aliases,
          baseAliasName,
          options,
        )}> &&result`,
      );
    } else {
      args.push('::React::ReactPromise<::React::JSValue> &&result');
    }
  }

  // TODO: be much more exhaustive on the possible method signatures that can be used..
  const sig = `REACT_${isMethodSync(funcType) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) ${translateImplReturnType(
    funcType.returnTypeAnnotation,
    aliases,
    baseAliasName,
    options,
  )} ${prop.name}(${args.join(', ')}) noexcept { /* implementation */ }`;

  const staticsig = `REACT_${isMethodSync(funcType) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) static ${translateImplReturnType(
    funcType.returnTypeAnnotation,
    aliases,
    baseAliasName,
    options,
  )} ${prop.name}(${args.join(', ')}) noexcept { /* implementation */ }`;

  return [sig, staticsig];
}

function translatePossibleMethodSignatures(
  prop: NativeModulePropertyShape,
  funcType: NativeModuleFunctionTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
): string {
  return getPossibleMethodSignatures(
    prop,
    funcType,
    aliases,
    baseAliasName,
    options,
  )
    .map(sig => `"    ${sig}\\n"`)
    .join('\n          ');
}

function renderProperties(
  properties: ReadonlyArray<NativeModulePropertyShape>,
  aliases: AliasMap,
  tuple: boolean,
  options: CppCodegenOptions,
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
        options,
      );

      const translatedReturnParam = translateSpecReturnType(
        funcType.returnTypeAnnotation,
        aliases,
        propAliasName,
        options,
      );

      if (funcType.returnTypeAnnotation.type === 'PromiseTypeAnnotation') {
        if (funcType.returnTypeAnnotation.elementType) {
          traversedArgs.push(
            `Promise<${translateSpecReturnType(
              funcType.returnTypeAnnotation.elementType,
              aliases,
              propAliasName,
              options,
            )}>`,
          );
        } else {
          traversedArgs.push('Promise<::React::JSValue>');
        }
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
            options,
          )});`;
      }
    })
    .join('\n');
}

export function generateValidateMethods(
  nativeModule: NativeModuleSchema,
  aliases: AliasMap,
  options: CppCodegenOptions,
): [string, string] {
  const properties = nativeModule.spec.properties;
  const traversedProperties = renderProperties(
    properties,
    aliases,
    false,
    options,
  );
  const traversedPropertyTuples = renderProperties(
    properties,
    aliases,
    true,
    options,
  );
  return [traversedPropertyTuples, traversedProperties];
}
