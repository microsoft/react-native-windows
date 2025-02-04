/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NativeModuleEventEmitterShape,
  NativeModuleFunctionTypeAnnotation,
  NativeModulePropertyShape,
  NativeModuleSchema,
} from '@react-native/codegen/lib/CodegenSchema';
import {AliasMap} from './AliasManaging';
import type {CppCodegenOptions} from './ObjectTypes';
import {
  translateArgs,
  translateSpecArgs,
  translateEventEmitterArgs,
} from './ParamTypes';
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
    // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
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
  methods: ReadonlyArray<NativeModulePropertyShape>,
  aliases: AliasMap,
  tuple: boolean,
  options: CppCodegenOptions,
): {code: string; numberOfProperties: number} {
  // TODO: generate code for constants
  const properties = methods
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
        // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
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
    });

  return {code: properties.join('\n'), numberOfProperties: properties.length};
}

function getPossibleEventEmitterSignatures(
  eventEmitter: NativeModuleEventEmitterShape,
  aliases: AliasMap,
  options: CppCodegenOptions,
): string[] {
  const traversedArgs = translateEventEmitterArgs(
    eventEmitter.typeAnnotation.typeAnnotation,
    aliases,
    eventEmitter.name,
    options,
  );
  return [
    `REACT_EVENT(${eventEmitter.name}) std::function<void(${traversedArgs})> ${eventEmitter.name};`,
  ];
}

function translatePossibleEventSignatures(
  eventEmitter: NativeModuleEventEmitterShape,
  aliases: AliasMap,
  options: CppCodegenOptions,
): string {
  return getPossibleEventEmitterSignatures(eventEmitter, aliases, options)
    .map(sig => `"    ${sig}\\n"`)
    .join('\n          ');
}

function renderEventEmitters(
  eventEmitters: ReadonlyArray<NativeModuleEventEmitterShape>,
  indexOffset: number,
  aliases: AliasMap,
  tuple: boolean,
  options: CppCodegenOptions,
): string {
  return eventEmitters
    .map((eventEmitter, index) => {
      const traversedArgs = translateEventEmitterArgs(
        eventEmitter.typeAnnotation.typeAnnotation,
        aliases,
        eventEmitter.name,
        options,
      );

      if (tuple) {
        return `      EventEmitter<void(${traversedArgs})>{${
          index + indexOffset
        }, L"${eventEmitter.name}"},`;
      } else {
        return `    REACT_SHOW_EVENTEMITTER_SPEC_ERRORS(
          ${index + indexOffset},
          "${eventEmitter.name}",
          ${translatePossibleEventSignatures(
            eventEmitter,
            aliases,
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
): {
  traversedProperties: string;
  traversedEventEmitters: string;
  traversedPropertyTuples: string;
  traversedEventEmitterTuples: string;
} {
  const methods = nativeModule.spec.methods;
  const eventEmitters = nativeModule.spec.eventEmitters;
  const traversedProperties = renderProperties(
    methods,
    aliases,
    false,
    options,
  );
  const traversedEventEmitters = renderEventEmitters(
    eventEmitters,
    traversedProperties.numberOfProperties,
    aliases,
    false,
    options,
  );
  const traversedPropertyTuples = renderProperties(
    methods,
    aliases,
    true,
    options,
  );
  const traversedEventEmitterTuples = renderEventEmitters(
    eventEmitters,
    traversedProperties.numberOfProperties,
    aliases,
    true,
    options,
  );
  return {
    traversedPropertyTuples: traversedPropertyTuples.code,
    traversedEventEmitterTuples,
    traversedProperties: traversedProperties.code,
    traversedEventEmitters,
  };
}
