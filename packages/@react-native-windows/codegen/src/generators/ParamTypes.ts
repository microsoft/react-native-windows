/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {
  NamedShape,
  NativeModuleParamTypeAnnotation,
  Nullable,
} from 'react-native-tscodegen';

type NativeModuleParamShape = NamedShape<
  Nullable<NativeModuleParamTypeAnnotation>
>;

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
      return decorateType(param.name, forSpec);
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

export function translateSpecArgs(
  params: ReadonlyArray<NativeModuleParamShape>,
) {
  return params.map(param => {
    const translatedParam = translateSpecFunctionParam(param);
    return `${translatedParam}`;
  });
}

export function translateArgs(params: ReadonlyArray<NativeModuleParamShape>) {
  return params.map(param => {
    const translatedParam = translateFunctionParam(param);
    return `${translatedParam} ${param.name}`;
  });
}
