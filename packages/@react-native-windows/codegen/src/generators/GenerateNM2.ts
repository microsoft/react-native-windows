/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {
  SchemaType,
  MethodTypeShape,
  // FunctionTypeAnnotation,
  FunctionTypeAnnotationParam,
  FunctionTypeAnnotationParamTypeAnnotation,
  FunctionTypeAnnotationReturn,
} from 'react-native-tscodegen';

type FilesOutput = Map<string, string>;

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

function translateSpecFunctionParam(
  param: FunctionTypeAnnotationParam,
): string {
  switch (param.typeAnnotation.type) {
    case 'StringTypeAnnotation':
      return 'std::string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'FunctionTypeAnnotation': {
      // Ideally we'd get more information about the expected parameters of the callback
      // But the current schema doesn't seem to provide the necessary information.
      return 'Callback<React::JSValue>';
    }
    case 'ArrayTypeAnnotation':
      // Ideally we'd get more information about the expected type of the array
      // But the current schema doesn't seem to provide the necessary information.
      return 'React::JSValueArray';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValueObject';
    case 'ObjectTypeAnnotation':
      // TODO we have more information here, and could create a more specific type
      return 'React::JSValueObject';
    case 'ReservedFunctionValueTypeAnnotation':
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (param.typeAnnotation.name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${param.typeAnnotation.name} in translateSpecFunctionParam`,
        );
      return 'double';
    default:
      throw new Error(
        `Unhandled type in translateSpecFunctionParam: ${param.typeAnnotation.type}`,
      );
  }
}

function translateFunctionParam(param: FunctionTypeAnnotationParam): string {
  switch (param.typeAnnotation.type) {
    case 'StringTypeAnnotation':
      return 'std::string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'FunctionTypeAnnotation': {
      // Ideally we'd get more information about the expected parameters of the callback
      // But the current schema doesn't seem to provide the necessary information.
      return 'std::function<void(React::JSValue const &)> const &';
    }
    case 'ArrayTypeAnnotation':
      // Ideally we'd get more information about the expected type of the array
      // But the current schema doesn't seem to provide the necessary information.
      return 'React::JSValueArray &&';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValueObject &&';
    case 'ObjectTypeAnnotation':
      // TODO we have more information here, and could create a more specific type
      return 'React::JSValueObject &&';
    case 'ReservedFunctionValueTypeAnnotation':
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (param.typeAnnotation.name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${param.typeAnnotation.name} in translateFunctionParam`,
        );
      return 'double';
    default:
      throw new Error(
        `Unhandled type in translateFunctionParam: ${param.typeAnnotation.type} in translateFunctionParam`,
      );
  }
}

function translateSpecReturnType(
  type:
    | FunctionTypeAnnotationParamTypeAnnotation
    | FunctionTypeAnnotationReturn,
) {
  switch (type.type) {
    case 'VoidTypeAnnotation':
      return 'void';
    case 'StringTypeAnnotation':
      return 'std::string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'GenericPromiseTypeAnnotation':
      return 'void';
    case 'ArrayTypeAnnotation':
      // Ideally we'd get more information about the expected type of the array
      // But the current schema doesn't seem to provide the necessary information.
      return 'React::JSValueArray';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValueObject';
    case 'ReservedFunctionValueTypeAnnotation':
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (type.name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${type.name} in translateSpecReturnType`,
        );
      return 'double';
    default:
      throw new Error(
        `Unhandled type in translateSpecReturnType: ${type.type}`,
      );
  }
}

function translateImplReturnType(
  type:
    | FunctionTypeAnnotationParamTypeAnnotation
    | FunctionTypeAnnotationReturn,
) {
  switch (type.type) {
    case 'VoidTypeAnnotation':
      return 'void';
    case 'StringTypeAnnotation':
      return 'std::string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'GenericPromiseTypeAnnotation':
      return 'void';
    case 'ArrayTypeAnnotation':
      // Ideally we'd get more information about the expected type of the array
      // But the current schema doesn't seem to provide the necessary information.
      return 'React::JSValueArray';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValueObject';
    case 'ReservedFunctionValueTypeAnnotation':
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (type.name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${type.name} in translateSpecReturnType`,
        );
      return 'double';
    default:
      throw new Error(
        `Unhandled type in translateImplReturnType: ${type.type}`,
      );
  }
}

function translateSpecArgs(params: ReadonlyArray<FunctionTypeAnnotationParam>) {
  return params.map(param => {
    const translatedParam = translateSpecFunctionParam(param);
    return `${translatedParam}`;
  });
}

function translateArgs(params: ReadonlyArray<FunctionTypeAnnotationParam>) {
  return params.map(param => {
    const translatedParam = translateFunctionParam(param);
    return `${translatedParam} ${param.name}`;
  });
}

function isMethodSync(prop: MethodTypeShape) {
  return (
    prop.typeAnnotation.returnTypeAnnotation.type !== 'VoidTypeAnnotation' &&
    prop.typeAnnotation.returnTypeAnnotation.type !==
      'GenericPromiseTypeAnnotation'
  );
}

function isPromise(prop: MethodTypeShape) {
  return (
    prop.typeAnnotation.returnTypeAnnotation.type ===
    'GenericPromiseTypeAnnotation'
  );
}

function getPossibleMethodSignatures(prop: MethodTypeShape): string[] {
  const args = translateArgs(prop.typeAnnotation.params);
  if (isPromise(prop)) {
    // Sadly, currently, the schema doesn't currently provide us information on the type of the promise.
    args.push('React::ReactPromise<React::JSValue> &&result');
  }

  // TODO be much more exhastive on the possible method signatures that can be used..
  const sig = `REACT_${isMethodSync(prop) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) ${translateImplReturnType(prop.typeAnnotation.returnTypeAnnotation)} ${
    prop.name
  }(${args.join(', ')}) noexcept { /* implementation */ }}`;

  const staticsig = `REACT_${isMethodSync(prop) ? 'SYNC_' : ''}METHOD(${
    prop.name
  }) static ${translateImplReturnType(
    prop.typeAnnotation.returnTypeAnnotation,
  )} ${prop.name}(${args.join(', ')}) noexcept { /* implementation */ }}`;

  return [sig, staticsig];
}

function translatePossibleMethodSignatures(prop: MethodTypeShape): string {
  return getPossibleMethodSignatures(prop)
    .map(sig => `"    ${sig}\\n"`)
    .join('\n          ');
}

function renderProperties(
  properties: ReadonlyArray<MethodTypeShape>,
  tuple: boolean,
): string {
  // We skip the constants for now, since we dont have Spec file validation of them.
  return properties
    .filter(prop => prop.name !== 'getConstants')
    .map((prop, index) => {
      const params = prop.typeAnnotation.params;

      const traversedArgs = translateSpecArgs(params);

      const translatedReturnParam = translateSpecReturnType(
        prop.typeAnnotation.returnTypeAnnotation,
      );

      if (isPromise(prop)) {
        // Sadly, currently, the schema doesn't currently provide us information on the type of the promise.
        traversedArgs.push('Promise<React::JSValue>');
      }

      if (tuple) {
        return `      ${
          isMethodSync(prop) ? 'Sync' : ''
        }Method<${translatedReturnParam}(${traversedArgs.join(
          ', ',
        )}) noexcept>{${index}, L"${prop.name}"},`;
      } else {
        return `    REACT_SHOW_METHOD_SPEC_ERRORS(
          ${index},
          "${prop.name}",
          ${translatePossibleMethodSignatures(prop)});`;
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

    const nativeModules = Object.keys(schema.modules)
      .map(moduleName => {
        const modules = schema.modules[moduleName].nativeModules;
        if (!modules) {
          throw new Error('modules does not exist');
        }

        return modules;
      })
      .filter(Boolean)
      .reduce((acc, components) => Object.assign(acc, components), {});

    Object.keys(nativeModules).forEach(name => {
      console.log(`Generating Native${name}Spec.g.h`);
      const {properties} = nativeModules[name];
      const traversedProperties = renderProperties(properties, false);
      const traversedPropertyTuples = renderProperties(properties, true);

      files.set(
        `Native${name}Spec.g.h`,
        moduleTemplate
          .replace(/::_MODULE_PROPERTIES_TUPLE_::/g, traversedPropertyTuples)
          .replace(/::_MODULE_PROPERTIES_SPEC_ERRORS_::/g, traversedProperties)
          .replace(/::_MODULE_NAME_::/g, name)
          .replace(/::_NAMESPACE_::/g, namespace),
      );
    });

    return files;
  };
}
