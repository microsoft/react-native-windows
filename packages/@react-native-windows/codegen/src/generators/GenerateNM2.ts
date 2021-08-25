/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {
  NamedShape,
  NativeModuleFunctionTypeAnnotation,
  NativeModuleParamTypeAnnotation,
  NativeModulePropertyShape,
  NativeModuleReturnTypeAnnotation,
  Nullable,
  SchemaType,
} from 'react-native-tscodegen';

type FilesOutput = Map<string, string>;
type NativeModuleParamShape = NamedShape<
  Nullable<NativeModuleParamTypeAnnotation>
>;

const moduleTemplate = `
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace ::_NAMESPACE_:: {

struct ::_MODULE_NAME_::Spec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
::_MODULE_PROPERTIES_TUPLE_::
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ::_MODULE_NAME_::Spec>();

::_MODULE_PROPERTIES_SPEC_ERRORS_::
  }
};

} // namespace ::_NAMESPACE_::
`;

function decorateType(type: string, forSpec: boolean): string {
  return forSpec ? type : `${type} &&`;
}

function translateParam(
  param: NativeModuleParamTypeAnnotation,
  forSpec: boolean,
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const paramType = param.type;
  switch (param.type) {
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
    case 'FunctionTypeAnnotation': {
      // TODO: type.params && type.returnTypeAnnotation
      if (forSpec) {
        return 'Callback<React::JSValue>';
      } else {
        return 'std::function<void(React::JSValue const &)> const &';
      }
    }
    case 'ArrayTypeAnnotation':
      // TODO: type.elementType
      return decorateType('React::JSValueArray', forSpec);
    case 'GenericObjectTypeAnnotation':
      return decorateType('React::JSValueObject', forSpec);
    case 'ObjectTypeAnnotation':
      // TODO: we have more information here, and could create a more specific type
      return decorateType('React::JSValueObject', forSpec);
    case 'ReservedTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = param.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(`Unknown reserved function: ${name} in translateParam`);
      return 'double';
    }
    case 'TypeAliasTypeAnnotation':
      // TODO: print the real name after processing NativeModuleSchema::aliases
      return 'React::JSValueObject';
    default:
      throw new Error(`Unhandled type in translateParam: ${paramType}`);
  }
}

function translateSpecFunctionParam(param: NativeModuleParamShape): string {
  switch (param.typeAnnotation.type) {
    case 'NullableTypeAnnotation':
      // TODO: should be
      // return `std::optional<${translateParam(
      //   param.typeAnnotation.typeAnnotation,
      //   true,
      // )}>`;
      return translateParam(param.typeAnnotation.typeAnnotation, true);
    default:
      return translateParam(param.typeAnnotation, true);
  }
}

function translateFunctionParam(param: NativeModuleParamShape): string {
  switch (param.typeAnnotation.type) {
    case 'NullableTypeAnnotation':
      // TODO: should be
      // return `std::optional<${translateParam(
      //   param.typeAnnotation.typeAnnotation,
      //   false,
      // )}>`;
      return translateParam(param.typeAnnotation.typeAnnotation, false);
    default:
      return translateParam(param.typeAnnotation, false);
  }
}

function translateReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
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
      // TODO: type.elementType
      return 'React::JSValueArray';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValueObject';
    case 'ObjectTypeAnnotation':
      // TODO: we have more information here, and could create a more specific type
      return 'React::JSValueObject';
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
      // TODO: print the real name after processing NativeModuleSchema::aliases
      return 'React::JSValueObject';
    case 'NullableTypeAnnotation':
      // TODO: should be `std::optional<${translateReturnType(type.typeAnnotation)}>`;
      return translateReturnType(type.typeAnnotation);
    default:
      throw new Error(`Unhandled type in translateReturnType: ${returnType}`);
  }
}

function translateSpecReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
) {
  return translateReturnType(type);
}

function translateImplReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
) {
  return translateReturnType(type);
}

function translateSpecArgs(params: ReadonlyArray<NativeModuleParamShape>) {
  return params.map(param => {
    const translatedParam = translateSpecFunctionParam(param);
    return `${translatedParam}`;
  });
}

function translateArgs(params: ReadonlyArray<NativeModuleParamShape>) {
  return params.map(param => {
    const translatedParam = translateFunctionParam(param);
    return `${translatedParam} ${param.name}`;
  });
}

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
): string[] {
  const args = translateArgs(funcType.params);
  if (isMethodReturnPromise(funcType)) {
    // Sadly, currently, the schema doesn't currently provide us information on the type of the promise.
    args.push('React::ReactPromise<React::JSValue> &&result');
  }

  // TODO: be much more exhastive on the possible method signatures that can be used..
  const sig = `REACT_${isMethodSync(funcType) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) ${translateImplReturnType(funcType.returnTypeAnnotation)} ${
    prop.name
  }(${args.join(', ')}) noexcept { /* implementation */ }}`;

  const staticsig = `REACT_${isMethodSync(funcType) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) static ${translateImplReturnType(funcType.returnTypeAnnotation)} ${
    prop.name
  }(${args.join(', ')}) noexcept { /* implementation */ }}`;

  return [sig, staticsig];
}

function translatePossibleMethodSignatures(
  prop: NativeModulePropertyShape,
  funcType: NativeModuleFunctionTypeAnnotation,
): string {
  return getPossibleMethodSignatures(prop, funcType)
    .map(sig => `"    ${sig}\\n"`)
    .join('\n          ');
}

function renderProperties(
  properties: ReadonlyArray<NativeModulePropertyShape>,
  tuple: boolean,
): string {
  // We skip the constants for now, since we dont have Spec file validation of them.
  return properties
    .filter(prop => prop.name !== 'getConstants')
    .map((prop, index) => {
      // TODO: prop.optional === true
      // TODO: prop.typeAnnotation.type === 'NullableTypeAnnotation'
      const funcType =
        prop.typeAnnotation.type === 'NullableTypeAnnotation'
          ? prop.typeAnnotation.typeAnnotation
          : prop.typeAnnotation;

      const traversedArgs = translateSpecArgs(funcType.params);

      const translatedReturnParam = translateSpecReturnType(
        funcType.returnTypeAnnotation,
      );

      if (isMethodReturnPromise(funcType)) {
        // Sadly, currently, the schema doesn't currently provide us information on the type of the promise.
        traversedArgs.push('Promise<React::JSValue>');
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
          ${translatePossibleMethodSignatures(prop, funcType)});`;
      }
    })
    .join('\n');
}

export function createNM2Generator({namespace}: {namespace: string}) {
  return (
    _libraryName: string,
    schema: SchemaType,
    _moduleSpecName: string,
  ): FilesOutput => {
    const files = new Map<string, string>();

    for (const moduleName of Object.keys(schema.modules)) {
      const nativeModule = schema.modules[moduleName];
      // from 0.65 facebook's react-native-codegen
      // the module name has the Native prefix comparing to 0.63
      // when reading files we provided
      const preferredModuleName = moduleName.startsWith('Native')
        ? moduleName.substr(6)
        : moduleName;

      if (nativeModule.type === 'NativeModule') {
        console.log(`Generating Native${preferredModuleName}Spec.g.h`);
        // TODO: Generate REACT_STRUCT(X) for nativeModule.aliases
        const properties = nativeModule.spec.properties;
        const traversedProperties = renderProperties(properties, false);
        const traversedPropertyTuples = renderProperties(properties, true);

        files.set(
          `Native${preferredModuleName}Spec.g.h`,
          moduleTemplate
            .replace(/::_MODULE_PROPERTIES_TUPLE_::/g, traversedPropertyTuples)
            .replace(
              /::_MODULE_PROPERTIES_SPEC_ERRORS_::/g,
              traversedProperties,
            )
            .replace(/::_MODULE_NAME_::/g, preferredModuleName)
            .replace(/::_NAMESPACE_::/g, namespace),
        );
      }
    }

    return files;
  };
}
