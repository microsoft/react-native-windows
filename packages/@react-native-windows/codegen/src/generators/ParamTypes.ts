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
import {getAliasCppName} from './ObjectTypes';

type NativeModuleParamShape = NamedShape<
  Nullable<NativeModuleParamTypeAnnotation>
>;

type ParamTarget = 'spec' | 'template' | 'callback-arg' | 'method-arg';

function decorateType(type: string, target: ParamTarget): string {
  switch (target) {
    case 'method-arg':
      return `${type} &&`;
    case 'callback-arg':
      return `${type} const &`;
    default:
      return type;
  }
}

function translateParam(
  param: NativeModuleParamTypeAnnotation,
  target: ParamTarget,
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
      // TODO: type.returnTypeAnnotation
      switch (target) {
        case 'spec':
          return `Callback<${param.params
            .map(translateSpecFunctionParam)
            .join(', ')}>`;
        case 'template':
          return `std::function<void(${param.params
            .map(translateCallbackParam)
            .join(', ')})>`;
        default:
          return `std::function<void(${param.params
            .map(translateCallbackParam)
            .join(', ')})> const &`;
      }
    }
    case 'ArrayTypeAnnotation':
      // TODO: type.elementType
      return decorateType('React::JSValueArray', target);
    case 'GenericObjectTypeAnnotation':
      return decorateType('React::JSValue', target);
    case 'ObjectTypeAnnotation':
      // TODO: we have more information here, and could create a more specific type
      return decorateType('React::JSValueObject', target);
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
      return decorateType(getAliasCppName(param.name), target);
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
      //   'spec',
      // )}>`;
      return translateParam(param.typeAnnotation.typeAnnotation, 'spec');
    default:
      return translateParam(param.typeAnnotation, 'spec');
  }
}

function translateCallbackParam(param: NativeModuleParamShape): string {
  switch (param.typeAnnotation.type) {
    case 'NullableTypeAnnotation':
      // TODO: should be
      // return `std::optional<${translateParam(
      //   param.typeAnnotation.typeAnnotation,
      //   'template',
      // )}>`;
      return translateParam(
        param.typeAnnotation.typeAnnotation,
        'callback-arg',
      );
    default:
      return translateParam(param.typeAnnotation, 'callback-arg');
  }
}

function translateFunctionParam(param: NativeModuleParamShape): string {
  switch (param.typeAnnotation.type) {
    case 'NullableTypeAnnotation':
      // TODO: should be
      // return `std::optional<${translateParam(
      //   param.typeAnnotation.typeAnnotation,
      //   'template',
      // )}>`;
      return translateParam(param.typeAnnotation.typeAnnotation, 'method-arg');
    default:
      return translateParam(param.typeAnnotation, 'method-arg');
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
