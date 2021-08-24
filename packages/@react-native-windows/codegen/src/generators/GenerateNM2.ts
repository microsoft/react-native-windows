/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {
  NativeModuleFunctionTypeAnnotation,
  NativeModuleMethodParamSchema,
  NativeModulePropertySchema,
  NativeModuleReturnTypeAnnotation,
  Nullable,
  SchemaType,
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
  param: NativeModuleMethodParamSchema,
): string {
  switch (param.typeAnnotation.type) {
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
      return 'Callback<React::JSValue>';
    }
    case 'ArrayTypeAnnotation':
      // TODO: type.elementType
      return 'React::JSValueArray';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValueObject';
    case 'ObjectTypeAnnotation':
      // TODO we have more information here, and could create a more specific type
      return 'React::JSValueObject';
    case 'ReservedFunctionValueTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = param.typeAnnotation.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${name} in translateSpecFunctionParam`,
        );
      return 'double';
    }
    // TODO: case 'TypeAliasTypeAnnotation':
    // TODO: case 'NullableTypeAnnotation':
    default:
      throw new Error(
        `Unhandled type in translateSpecFunctionParam: ${param.typeAnnotation.type}`,
      );
  }
}

function translateFunctionParam(param: NativeModuleMethodParamSchema): string {
  switch (param.typeAnnotation.type) {
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
      return 'std::function<void(React::JSValue const &)> const &';
    }
    case 'ArrayTypeAnnotation':
      // TODO: type.elementType
      return 'React::JSValueArray &&';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValueObject &&';
    case 'ObjectTypeAnnotation':
      // TODO we have more information here, and could create a more specific type
      return 'React::JSValueObject &&';
    case 'ReservedFunctionValueTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = param.typeAnnotation.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${name} in translateFunctionParam`,
        );
      return 'double';
    }
    // TODO: case 'TypeAliasTypeAnnotation':
    // TODO: case 'NullableTypeAnnotation':
    default:
      throw new Error(
        `Unhandled type in translateFunctionParam: ${param.typeAnnotation.type} in translateFunctionParam`,
      );
  }
}

function translateSpecReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
) {
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
    case 'ReservedFunctionValueTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = type.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${name} in translateSpecReturnType`,
        );
      return 'double';
    }
    // TODO: case 'ObjectTypeAnnotation':
    // TODO: case 'TypeAliasTypeAnnotation':
    // TODO: case 'NullableTypeAnnotation':
    default:
      throw new Error(
        `Unhandled type in translateSpecReturnType: ${type.type}`,
      );
  }
}

function translateImplReturnType(
  type: Nullable<NativeModuleReturnTypeAnnotation>,
) {
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
    case 'ReservedFunctionValueTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = type.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${name} in translateSpecReturnType`,
        );
      return 'double';
    }
    // TODO: case 'ObjectTypeAnnotation':
    // TODO: case 'TypeAliasTypeAnnotation':
    // TODO: case 'NullableTypeAnnotation':
    default:
      throw new Error(
        `Unhandled type in translateImplReturnType: ${type.type}`,
      );
  }
}

function translateSpecArgs(
  params: ReadonlyArray<NativeModuleMethodParamSchema>,
) {
  return params.map(param => {
    const translatedParam = translateSpecFunctionParam(param);
    return `${translatedParam}`;
  });
}

function translateArgs(params: ReadonlyArray<NativeModuleMethodParamSchema>) {
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
  prop: NativeModulePropertySchema,
  funcType: NativeModuleFunctionTypeAnnotation,
): string[] {
  const args = translateArgs(funcType.params);
  if (isMethodReturnPromise(funcType)) {
    // Sadly, currently, the schema doesn't currently provide us information on the type of the promise.
    args.push('React::ReactPromise<React::JSValue> &&result');
  }

  // TODO be much more exhastive on the possible method signatures that can be used..
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
  prop: NativeModulePropertySchema,
  funcType: NativeModuleFunctionTypeAnnotation,
): string {
  return getPossibleMethodSignatures(prop, funcType)
    .map(sig => `"    ${sig}\\n"`)
    .join('\n          ');
}

function renderProperties(
  properties: ReadonlyArray<NativeModulePropertySchema>,
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
      if (nativeModule.type === 'NativeModule') {
        console.log(`Generating Native${moduleName}Spec.g.h`);
        // TODO: Generate REACT_STRUCT(X) for nativeModule.aliases
        const properties = nativeModule.spec.properties;
        const traversedProperties = renderProperties(properties, false);
        const traversedPropertyTuples = renderProperties(properties, true);

        files.set(
          `Native${moduleName}Spec.g.h`,
          moduleTemplate
            .replace(/::_MODULE_PROPERTIES_TUPLE_::/g, traversedPropertyTuples)
            .replace(
              /::_MODULE_PROPERTIES_SPEC_ERRORS_::/g,
              traversedProperties,
            )
            .replace(/::_MODULE_NAME_::/g, moduleName)
            .replace(/::_NAMESPACE_::/g, namespace),
        );
      }
    }

    return files;
  };
}
