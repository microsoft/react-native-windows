/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NamedShape,
  NativeModuleArrayTypeAnnotation,
  NativeModuleBaseTypeAnnotation,
  NativeModuleEventEmitterBaseTypeAnnotation,
  NativeModuleEventEmitterTypeAnnotation,
  NativeModuleEnumDeclaration,
  NativeModuleFunctionTypeAnnotation,
  NativeModuleParamTypeAnnotation,
  NativeModuleUnionTypeAnnotation,
  UnsafeAnyTypeAnnotation,
  Nullable,
} from '@react-native/codegen/lib/CodegenSchema';
import {
  AliasMap,
  getAliasCppName,
  getAnonymousAliasCppName,
} from './AliasManaging';
import type {CppCodegenOptions} from './ObjectTypes';

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

function translateUnionReturnType(
  type: NativeModuleEnumDeclaration | NativeModuleUnionTypeAnnotation,
  target: ParamTarget,
  options: CppCodegenOptions,
): string {
  const memberType = type.memberType;
  switch (type.memberType) {
    case 'StringTypeAnnotation':
      return options.cppStringType;
    case 'NumberTypeAnnotation':
      return 'double';
    case 'ObjectTypeAnnotation':
      return decorateType('::React::JSValue', target);
    default:
      throw new Error(
        `Unknown enum/union member type in translateReturnType: ${memberType}`,
      );
  }
}

function translateFunction(
  param: NativeModuleFunctionTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  target: ParamTarget,
  options: CppCodegenOptions,
): string {
  // TODO: type.returnTypeAnnotation
  switch (target) {
    case 'spec':
      return `Callback<${param.params
        .map((p: NativeModuleParamShape) =>
          translateSpecFunctionParam(
            p,
            aliases,
            `${baseAliasName}_${p.name}`,
            options,
          ),
        )
        .join(', ')}>`;
    case 'template':
      return `std::function<void(${param.params
        .map((p: NativeModuleParamShape) =>
          translateCallbackParam(
            p,
            aliases,
            `${baseAliasName}_${p.name}`,
            options,
          ),
        )
        .join(', ')})>`;
    default:
      return `std::function<void(${param.params
        .map((p: NativeModuleParamShape) =>
          translateCallbackParam(
            p,
            aliases,
            `${baseAliasName}_${p.name}`,
            options,
          ),
        )
        .join(', ')})> const &`;
  }
}

function translateArray(
  param: NativeModuleArrayTypeAnnotation<
    Nullable<NativeModuleBaseTypeAnnotation>
  >,
  aliases: AliasMap,
  baseAliasName: string,
  target: ParamTarget,
  options: CppCodegenOptions,
): string {
  if (param.elementType.type !== 'AnyTypeAnnotation') {
    switch (target) {
      case 'spec':
      case 'template':
        return `std::vector<${translateNullableParamType(
          param.elementType,
          aliases,
          `${baseAliasName}_element`,
          'template',
          'template',
          options,
        )}>`;
      default:
        return `std::vector<${translateNullableParamType(
          param.elementType,
          aliases,
          `${baseAliasName}_element`,
          'template',
          'template',
          options,
        )}> const &`;
    }
  } else {
    return decorateType('::React::JSValueArray', target);
  }
}

// Hopefully eventually NativeModuleEventEmitterTypeAnnotation will align better with NativeModuleParamTypeAnnotation
// and this method can be merged / replaced with translateArray
function translateEventEmitterArray(
  param: {
    readonly type: 'ArrayTypeAnnotation';
    readonly elementType:
      | NativeModuleEventEmitterBaseTypeAnnotation
      | {type: string};
  },
  aliases: AliasMap,
  baseAliasName: string,
  target: ParamTarget,
  options: CppCodegenOptions,
): string {
  switch (target) {
    case 'spec':
    case 'template':
      return `std::vector<${translateEventEmitterParam(
        param.elementType as NativeModuleEventEmitterBaseTypeAnnotation,
        aliases,
        `${baseAliasName}_element`,
        'template',
        options,
      )}>`;
    default:
      return `std::vector<${translateEventEmitterParam(
        param.elementType as NativeModuleEventEmitterBaseTypeAnnotation,
        aliases,
        `${baseAliasName}_element`,
        'template',
        options,
      )}> const &`;
  }
}

function translateParam(
  param: NativeModuleParamTypeAnnotation | UnsafeAnyTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  target: ParamTarget,
  options: CppCodegenOptions,
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const paramType = param.type;
  switch (param.type) {
    case 'StringTypeAnnotation':
      return options.cppStringType;
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
    case 'DoubleTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'FunctionTypeAnnotation':
      return translateFunction(param, aliases, baseAliasName, target, options);
    case 'ArrayTypeAnnotation':
      return translateArray(param, aliases, baseAliasName, target, options);
    case 'GenericObjectTypeAnnotation':
      return decorateType('::React::JSValue', target);
    case 'ObjectTypeAnnotation':
      return decorateType(
        getAnonymousAliasCppName(aliases, baseAliasName, param),
        target,
      );
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
    case 'MixedTypeAnnotation':
      return '';
    case 'EnumDeclaration':
    case 'UnionTypeAnnotation':
      return translateUnionReturnType(param, target, options);
    case 'AnyTypeAnnotation':
      return decorateType('::React::JSValue', target);
    default:
      throw new Error(`Unhandled type in translateParam: ${paramType}`);
  }
}

// Hopefully eventually NativeModuleEventEmitterTypeAnnotation will align better with NativeModuleParamTypeAnnotation
// and this method can be merged / replaced with translateParam
function translateEventEmitterParam(
  param: NativeModuleEventEmitterTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  target: ParamTarget,
  options: CppCodegenOptions,
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const paramType = param.type;
  switch (param.type) {
    case 'StringTypeAnnotation':
      return options.cppStringType;
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
    case 'DoubleTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'ArrayTypeAnnotation':
      return translateEventEmitterArray(
        param,
        aliases,
        baseAliasName,
        target,
        options,
      );
    case 'TypeAliasTypeAnnotation':
      return decorateType(getAliasCppName(param.name), target);
    case 'VoidTypeAnnotation':
      return 'void';
    default:
      throw new Error(`Unhandled type in translateParam: ${paramType}`);
  }
}

function translateNullableParamType(
  paramType:
    | Nullable<NativeModuleParamTypeAnnotation>
    | UnsafeAnyTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  nullableTarget: ParamTarget,
  target: ParamTarget,
  options: CppCodegenOptions,
): string {
  switch (paramType.type) {
    case 'NullableTypeAnnotation':
      return `std::optional<${translateParam(
        paramType.typeAnnotation,
        aliases,
        baseAliasName,
        nullableTarget,
        options,
      )}>`;
    default:
      return translateParam(paramType, aliases, baseAliasName, target, options);
  }
}

function translateSpecFunctionParam(
  param: NativeModuleParamShape,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
): string {
  return translateNullableParamType(
    param.typeAnnotation,
    aliases,
    baseAliasName,
    'spec',
    'spec',
    options,
  );
}

function translateCallbackParam(
  param: NativeModuleParamShape,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
): string {
  return translateNullableParamType(
    param.typeAnnotation,
    aliases,
    baseAliasName,
    'template',
    'callback-arg',
    options,
  );
}

function translateFunctionParam(
  param: NativeModuleParamShape,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
): string {
  return translateNullableParamType(
    param.typeAnnotation,
    aliases,
    baseAliasName,
    'template',
    'method-arg',
    options,
  );
}

export function translateSpecArgs(
  params: ReadonlyArray<NativeModuleParamShape>,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
) {
  return params.map(param => {
    const translatedParam = translateSpecFunctionParam(
      param,
      aliases,
      `${baseAliasName}_${param.name}`,
      options,
    );
    return `${translatedParam}`;
  });
}

export function translateEventEmitterArgs(
  params: NativeModuleEventEmitterTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
) {
  const translatedParam = translateEventEmitterParam(
    params,
    aliases,
    baseAliasName,
    'spec',
    options,
  );
  return `${translatedParam}`;
}

export function translateArgs(
  params: ReadonlyArray<NativeModuleParamShape>,
  aliases: AliasMap,
  baseAliasName: string,
  options: CppCodegenOptions,
) {
  return params.map(param => {
    const translatedParam = translateFunctionParam(
      param,
      aliases,
      `${baseAliasName}_${param.name}`,
      options,
    );
    return `${translatedParam} ${param.name}`;
  });
}
